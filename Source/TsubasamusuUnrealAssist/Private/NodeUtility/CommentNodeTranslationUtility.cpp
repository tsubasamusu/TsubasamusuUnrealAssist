// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "NodeUtility/CommentTranslationUtility.h"
#include "EdGraphNode_Comment.h"
#include "TsubasamusuLogUtility.h"
#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/TextLocalizationManager.h"
#include "HttpModule.h"
#include "Setting/TsubasamusuUnrealAssistSettings.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Setting/EditorSettingsUtility.h"

#define LOCTEXT_NAMESPACE "TsubasamusuUnrealAssist"

void FCommentTranslationUtility::AddCommentTranslationMenu(FMenuBuilder& InMenuBuilder, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
    const FName ExtensionHookName = TEXT("TsubasamusuUnrealAssistSection");
    const TAttribute<FText> HeadingText = LOCTEXT("CommentTranslationHeading", "Tsubasamusu Unreal Assist");
    
	InMenuBuilder.BeginSection(ExtensionHookName, HeadingText);

    const TAttribute<FText> LabelText = LOCTEXT("CommentTranslationLabel", "Translate to...");
    const TAttribute<FText> ToolTipText = LOCTEXT("CommentTranslationToolTip", "Translate comment of selected comment node.");

    const auto MenuAction = [InCommentNode](FMenuBuilder& MenuBuilder)
    {
        AddLanguageSubMenus(MenuBuilder, InCommentNode);
    };
    
    InMenuBuilder.AddSubMenu(LabelText, ToolTipText, FNewMenuDelegate::CreateLambda(MenuAction), FUIAction(), NAME_None, EUserInterfaceActionType::None);

    InMenuBuilder.EndSection();
}

void FCommentTranslationUtility::AddLanguageSubMenus(FMenuBuilder& InMenuBuilder, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
    const TArray<FString> EditorLanguages = GetEditorLanguages();

    for (const FString& EditorLanguage : EditorLanguages)
    {
        if (EditorLanguage.IsEmpty() || EditorLanguage == TEXT("es-419"))
        {
            continue;
        }
        
        FInternationalization& Internationalization = FInternationalization::Get();
        FCulturePtr Culture = Internationalization.GetCulture(EditorLanguage);
        
        if (!Culture.IsValid())
        {
            continue;
        }

        const TAttribute<FText> LabelText = FText::FromString(Culture->GetEnglishName());
        const TAttribute<FText> ToolTipText = FText::Format(LOCTEXT("LanguageSubMenuToolTip", "Translate comment of selected comment node to {0}."), FText::FromString(Culture->GetEnglishName()));
        
        const TSharedPtr<const FString> TranslationTargetLanguage = MakeShared<FString>(EditorLanguage);
        
        InMenuBuilder.AddMenuEntry(LabelText, ToolTipText, FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([InCommentNode, TranslationTargetLanguage]()
        {
            TranslateComment(InCommentNode, TranslationTargetLanguage);
        })));
    }
}

TArray<FString> FCommentTranslationUtility::GetEditorLanguages()
{
    const FTextLocalizationManager& TextLocalizationManager = FTextLocalizationManager::Get();
    const TArray<FString> LocalizedCultureNames = TextLocalizationManager.GetLocalizedCultureNames(ELocalizationLoadFlags::Editor);

    FInternationalization& Internationalization = FInternationalization::Get();
    TArray<FCultureRef> AvailableCultures = Internationalization.GetAvailableCultures(LocalizedCultureNames, false);

    TArray<FString> EditorLanguages;
    
    for (const FCultureRef& AvailableCulture : AvailableCultures)
    {
        EditorLanguages.Add(AvailableCulture->GetName());
    }
    
    return EditorLanguages;
}

void FCommentTranslationUtility::TranslateComment(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode, const TSharedPtr<const FString> TranslationTargetLanguage)
{
    if (!InCommentNode.IsValid() || !TranslationTargetLanguage.IsValid())
    {
        return;
    }

    const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
    const FString ApiKey = TsubasamusuUnrealAssistSettings->DeeplApiKey;
    
    const FString DeeplJsonRequest = GetDeeplJsonRequest(InCommentNode->NodeComment, *TranslationTargetLanguage);
    
    const TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
    
    HttpRequest->SetURL(TEXT("https://api-free.deepl.com/v2/translate"));
    HttpRequest->SetVerb(TEXT("POST"));
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    HttpRequest->SetHeader(TEXT("Authorization"), TEXT("DeepL-Auth-Key ") + ApiKey);
    HttpRequest->SetContentAsString(DeeplJsonRequest);

    HttpRequest->OnProcessRequestComplete().BindLambda([InCommentNode](FHttpRequestPtr, FHttpResponsePtr HttpResponse, const bool bSucceeded)
    {
        if (!bSucceeded)
        {
            TUA_ERROR(TEXT("Failed to send a HTTP request."));

            return;
        }

        if (!HttpResponse.IsValid())
        {
            TUA_ERROR(TEXT("Failed to get a HTTP response."));

            return;
        }
        
        const FString JsonResponse = HttpResponse->GetContentAsString();
        
        TSharedPtr<FJsonObject> JsonObject;
        const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonResponse);
        
        if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
        {
            TUA_ERROR(TEXT("Failed to deserialize the HTTP response \"%s.\""), *JsonResponse);

            return;
        }
        
        const TArray<TSharedPtr<FJsonValue>>* Translations;
        
        if (!JsonObject->TryGetArrayField(TEXT("translations"), Translations) || Translations->Num() == 0)
        {
            TUA_ERROR(TEXT("Failed to get translations from the HTTP response \"%s.\""), *JsonResponse);

            return;
        }
        
        const TSharedPtr<FJsonObject> TranslationJsonObject = (*Translations)[0]->AsObject();
        
        FString TranslatedText;
        
        if (!TranslationJsonObject->TryGetStringField(TEXT("text"), TranslatedText))
        {
            TUA_ERROR(TEXT("Failed to get a translated text from the HTTP response \"%s.\""), *JsonResponse);

            return;
        }
        
        if (InCommentNode.IsValid())
        {
            const FScopedTransaction Transaction(LOCTEXT("CommentTranslationTransaction", "Translate Comment"));

            InCommentNode->Modify();
            InCommentNode->NodeComment = TranslatedText;
            InCommentNode->ReconstructNode();

            UEdGraph* Graph = InCommentNode->GetGraph();
            
            if (IsValid(Graph))
            {
                Graph->NotifyGraphChanged();
            }
        }
    });
    
    if (!HttpRequest->ProcessRequest())
    {
        TUA_ERROR(TEXT("Failed to process a HTTP request."));
    }
}

FString FCommentTranslationUtility::GetDeeplJsonRequest(const FString& SourceText, const FString& TargetLanguage)
{
    const TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    const TArray<TSharedPtr<FJsonValue>> SourceTexts = { MakeShared<FJsonValueString>(SourceText) };

    FString FixedLanguage = TargetLanguage;
    FixLanguage(FixedLanguage);
    
    JsonObject->SetArrayField(TEXT("text"), SourceTexts);
    JsonObject->SetStringField(TEXT("target_lang"), FixedLanguage);

    FString DeeplJsonRequest;
    
    const TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&DeeplJsonRequest);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);
    
    return DeeplJsonRequest;
}

void FCommentTranslationUtility::FixLanguage(FString& OutLanguage)
{
    if (OutLanguage == TEXT("es-419"))
    {
        OutLanguage = TEXT("es");
    }
    
    OutLanguage = OutLanguage.ToUpper();
}

#undef LOCTEXT_NAMESPACE
