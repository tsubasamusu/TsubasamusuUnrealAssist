// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "Blueprint/CommentTranslator.h"
#include "EdGraphNode_Comment.h"
#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/TextLocalizationManager.h"
#include "HttpModule.h"
#include "NodeInformationUtility.h"
#include "TsubasamusuStringUtility.h"
#include "Setting/TsubasamusuUnrealAssistSettings.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Setting/EditorSettingsUtility.h"
#include "Type/TsubasamusuUnrealAssistStructs.h"

#define LOCTEXT_NAMESPACE "FCommentTranslator"

FSelectedNodeMenuContext FCommentTranslator::CreateSelectedNodeMenuContext()
{
    const FShouldAddMenu ShouldAddMenu = [](const TArray<TWeakObjectPtr<UEdGraphNode>>& InSelectedNodes)
    {
        if (InSelectedNodes.Num() == 1)
        {
            const TWeakObjectPtr<UEdGraphNode> SelectedNode = InSelectedNodes[0];
			
            if (SelectedNode.IsValid())
            {
                return FNodeInformationUtility::IsCommentNode(SelectedNode.Get()) && !SelectedNode->NodeComment.IsEmpty();
            }
        }
		
        return false;
    };
	
    return FSelectedNodeMenuContext
    {
        .ShouldAddMenu = ShouldAddMenu,
        .LabelText = LOCTEXT("CommentTranslationLabel", "Translate to..."),
        .ToolTipText = LOCTEXT("CommentTranslationToolTip", "Translate comment of selected comment node."),
        .SubMenuContexts = CreateSelectedNodeSubMenuContext()
    };
}

TArray<FSelectedNodeSubMenuContext> FCommentTranslator::CreateSelectedNodeSubMenuContext()
{
    TArray<FSelectedNodeSubMenuContext> SubMenuContexts;
    const TMap<FString, FText> EditorLanguages = GetEditorLanguages();
    
    const FOnSelectedNodeSubMenuClicked OnSubMenuClicked = [EditorLanguages](const TArray<TWeakObjectPtr<UEdGraphNode>>& InSelectedNodes, const TWeakObjectPtr<UEdGraph>, const FText& InSubMenuLabelText)
    {
        if (InSelectedNodes.Num() == 1)
        {
            const TWeakObjectPtr<UEdGraphNode> SelectedNode = InSelectedNodes[0];
            
            if (SelectedNode.IsValid())
            {
                const TWeakObjectPtr<UEdGraphNode_Comment> CommentNode = Cast<UEdGraphNode_Comment>(SelectedNode);
                
                for (const TPair<FString, FText>& EditorLanguage : EditorLanguages)
                {
                    if (!EditorLanguage.Value.IsEmpty() && EditorLanguage.Value.EqualTo(InSubMenuLabelText))
                    {
                        TranslateComment(CommentNode, *EditorLanguage.Key);
                    }
                }
            }
        }
    };
    
    for (const TPair<FString, FText>& EditorLanguage : EditorLanguages)
    {
        if (!EditorLanguage.Key.IsEmpty() && EditorLanguage.Key != TEXT("es-419"))
        {
            const FText ToolTipText = FText::Format(LOCTEXT("LanguageSubMenuToolTip", "Translate comment of selected comment node to {0}."), EditorLanguage.Value);
            
            FSelectedNodeSubMenuContext SubMenuContext
            {
                .OnClicked = OnSubMenuClicked,
                .LabelText = EditorLanguage.Value,
                .ToolTipText = ToolTipText,
            };
            
            SubMenuContexts.Add(SubMenuContext);
        }
    }
    
    return SubMenuContexts;
}

TMap<FString, FText> FCommentTranslator::GetEditorLanguages()
{
    TMap<FString, FText> EditorLanguages;
    
    FInternationalization& Internationalization = FInternationalization::Get();
    const TArray<FString> LocalizedCultureNames = FTextLocalizationManager::Get().GetLocalizedCultureNames(ELocalizationLoadFlags::Editor);
    TArray<FCultureRef> AvailableCultures = Internationalization.GetAvailableCultures(LocalizedCultureNames, false);

    for (const FCultureRef& AvailableCulture : AvailableCultures)
    {
        const FString LanguageString = AvailableCulture->GetName();
        const FCulturePtr Culture = Internationalization.GetCulture(LanguageString);
        
        if (Culture.IsValid())
        {
            const FText LanguageText = FText::FromString(Culture->GetEnglishName());
            EditorLanguages.Add(LanguageString, LanguageText);
        }
    }
    
    return EditorLanguages;
}

void FCommentTranslator::TranslateComment(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode, const FString& InTranslationTargetLanguage)
{
    if (InCommentNode.IsValid() && !InTranslationTargetLanguage.IsEmpty())
    {
        const TSharedPtr<FString> SourceComment = MakeShared<FString>(InCommentNode->NodeComment);
        const TSharedPtr<FJsonObject> RequestJsonObject = MakeShared<FJsonObject>();
        const TArray<TSharedPtr<FJsonValue>> SourceTextJsonValues = { MakeShared<FJsonValueString>(*SourceComment) };

        FString TranslationTargetLanguage = InTranslationTargetLanguage;
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
        
        auto AnimationTextChangedFunction = [InCommentNode](const FString& InAnimationText)
        {
            if (InCommentNode.IsValid())
            {
                InCommentNode->OnUpdateCommentText(InAnimationText);
            }
        };
		
        const TSharedRef<FTSTicker::FDelegateHandle> TickerHandle = FTsubasamusuStringUtility::PlayTextAnimation(TEXT("Translating"), AnimationTextChangedFunction);

        HttpRequest->OnProcessRequestComplete().BindLambda([InCommentNode, TickerHandle, SourceComment](FHttpRequestPtr, FHttpResponsePtr InHttpResponsePtr, const bool bInProcessedSuccessfully)
        {
            if (TickerHandle->IsValid())
            {
                FTSTicker::GetCoreTicker().RemoveTicker(*TickerHandle);
                TickerHandle->Reset();
            }
            
            if(InCommentNode.IsValid())
            {
                if (bInProcessedSuccessfully && InHttpResponsePtr.IsValid())
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
                                return;
                            }
                        }
                    }
                }
                
                InCommentNode->OnUpdateCommentText(*SourceComment);
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
