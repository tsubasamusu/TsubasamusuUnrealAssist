// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "Blueprint/CommentTranslator.h"
#include "EdGraphNode_Comment.h"
#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/TextLocalizationManager.h"
#include "HttpModule.h"
#include "Setting/TsubasamusuUnrealAssistSettings.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Setting/EditorSettingsUtility.h"

#define LOCTEXT_NAMESPACE "FCommentTranslator"

void FCommentTranslator::AddCommentTranslationMenu(FMenuBuilder& InMenuBuilder, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
    const FName ExtensionHookName = TEXT("TsubasamusuUnrealAssistSection");
    const TAttribute<FText> HeadingText = LOCTEXT("CommentTranslationHeading", "Tsubasamusu Unreal Assist");
    
	InMenuBuilder.BeginSection(ExtensionHookName, HeadingText);

    const TAttribute<FText> LabelText = LOCTEXT("CommentTranslationLabel", "Translate to...");
    const TAttribute<FText> ToolTipText = LOCTEXT("CommentTranslationToolTip", "Translate comment of selected comment node.");

    const auto MenuAction = [InCommentNode](FMenuBuilder& InSubMenuBuilder)
    {
        AddLanguageSubMenus(InSubMenuBuilder, InCommentNode);
    };
    
    InMenuBuilder.AddSubMenu(LabelText, ToolTipText, FNewMenuDelegate::CreateLambda(MenuAction), FUIAction(), NAME_None, EUserInterfaceActionType::None);
    InMenuBuilder.EndSection();
}

void FCommentTranslator::AddLanguageSubMenus(FMenuBuilder& InMenuBuilder, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
    const TArray<FString> EditorLanguages = GetEditorLanguages();

    for (const FString& EditorLanguage : EditorLanguages)
    {
        if (!EditorLanguage.IsEmpty() && EditorLanguage != TEXT("es-419"))
        {
            FCulturePtr Culture = FInternationalization::Get().GetCulture(EditorLanguage);
        
            if (Culture.IsValid())
            {
                const FString CultureString = Culture->GetEnglishName();
                const FText CultureText = FText::FromString(CultureString);
                
                const TAttribute<FText> ToolTipText = FText::Format(LOCTEXT("LanguageSubMenuToolTip", "Translate comment of selected comment node to {0}."), CultureText);
                const TSharedPtr<const FString> TranslationTargetLanguage = MakeShared<FString>(EditorLanguage);
        
                InMenuBuilder.AddMenuEntry(CultureText, ToolTipText, FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([InCommentNode, TranslationTargetLanguage]()
                {
                    TranslateComment(InCommentNode, TranslationTargetLanguage);
                })));
            }
        }
    }
}

TArray<FString> FCommentTranslator::GetEditorLanguages()
{
    TArray<FString> EditorLanguages;
    
    const TArray<FString> LocalizedCultureNames = FTextLocalizationManager::Get().GetLocalizedCultureNames(ELocalizationLoadFlags::Editor);
    TArray<FCultureRef> AvailableCultures = FInternationalization::Get().GetAvailableCultures(LocalizedCultureNames, false);

    for (const FCultureRef& AvailableCulture : AvailableCultures)
    {
        EditorLanguages.Add(AvailableCulture->GetName());
    }
    
    return EditorLanguages;
}

void FCommentTranslator::TranslateComment(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode, const TSharedPtr<const FString> InTranslationTargetLanguage)
{
    if (InCommentNode.IsValid() && InTranslationTargetLanguage.IsValid())
    {
        const TSharedPtr<FJsonObject> RequestJsonObject = MakeShared<FJsonObject>();
        const TArray<TSharedPtr<FJsonValue>> SourceTextJsonValues = { MakeShared<FJsonValueString>(InCommentNode->NodeComment) };

        FString TranslationTargetLanguage = *InTranslationTargetLanguage;
        FixLanguage(TranslationTargetLanguage);
    
        RequestJsonObject->SetArrayField(TEXT("text"), SourceTextJsonValues);
        RequestJsonObject->SetStringField(TEXT("target_lang"), TranslationTargetLanguage);

        FString RequestContent;
        const TSharedRef<TJsonWriter<>> RequestJsonWriter = TJsonWriterFactory<>::Create(&RequestContent);
        FJsonSerializer::Serialize(RequestJsonObject.ToSharedRef(), RequestJsonWriter);
    
        const TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
    
        const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
        const FString Authorization = FString::Printf(TEXT("DeepL-Auth-Key %s"), *TsubasamusuUnrealAssistSettings->DeeplApiKey);
    
        HttpRequest->SetURL(TEXT("https://api-free.deepl.com/v2/translate"));
        HttpRequest->SetVerb(TEXT("POST"));
        HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
        HttpRequest->SetHeader(TEXT("Authorization"), Authorization);
        HttpRequest->SetContentAsString(RequestContent);

        HttpRequest->OnProcessRequestComplete().BindLambda([InCommentNode](FHttpRequestPtr, FHttpResponsePtr InHttpResponsePtr, const bool bInProcessedSuccessfully)
        {
            if (bInProcessedSuccessfully && InHttpResponsePtr.IsValid() && InCommentNode.IsValid())
            {
                const FString ResponseContent = InHttpResponsePtr->GetContentAsString();
                TSharedPtr<FJsonObject> ResponseJsonObject;
                const TSharedRef<TJsonReader<>> ResponseJsonReader = TJsonReaderFactory<>::Create(ResponseContent);
        
                if (FJsonSerializer::Deserialize(ResponseJsonReader, ResponseJsonObject))
                {
                    const TArray<TSharedPtr<FJsonValue>>* TranslationJsonValues;
        
                    if (ResponseJsonObject->TryGetArrayField(TEXT("translations"), TranslationJsonValues) && TranslationJsonValues && !TranslationJsonValues->IsEmpty())
                    {
                        const TSharedPtr<FJsonObject> TranslationJsonObject = (*TranslationJsonValues)[0]->AsObject();
                        FString TranslatedText;
        
                        if (TranslationJsonObject->TryGetStringField(TEXT("text"), TranslatedText) && !TranslatedText.IsEmpty())
                        {
                            InCommentNode->OnUpdateCommentText(*TranslatedText);
                        }
                    }
                }
            }
        });
    
        HttpRequest->ProcessRequest();
    }
}

void FCommentTranslator::FixLanguage(FString& OutLanguage)
{
    if (OutLanguage == TEXT("es-419"))
    {
        OutLanguage = TEXT("es");
    }
    
    OutLanguage = OutLanguage.ToUpper();
}

#undef LOCTEXT_NAMESPACE
