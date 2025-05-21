// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "CommentNodeTranslationUtility.h"
#include "EdGraphNode_Comment.h"
#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/TextLocalizationManager.h"

#define LOCTEXT_NAMESPACE "FCommentNodeTranslationUtility"

void FCommentNodeTranslationUtility::AddCommentNodeTranslationMenu(FMenuBuilder& InMenuBuilder, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
    const FName ExtensionHookName = TEXT("TsubasamusuUnrealAssistSection");
    const TAttribute HeadingText = LOCTEXT("TsubasamusuUnrealAssistSectionHeader", "Tsubasamusu Unreal Assist");
    
	InMenuBuilder.BeginSection(ExtensionHookName, HeadingText);

    const TAttribute LabelText = LOCTEXT("CommentNodeTranslationLabelText", "Translate to...");
    const TAttribute ToolTipText = LOCTEXT("CommentNodeTranslationToolTipText", "Translate comment of selected comment node.");

    const auto MenuAction = [InCommentNode](FMenuBuilder& MenuBuilder)
    {
        AddLanguageSubMenus(MenuBuilder, InCommentNode);
    };
    
    InMenuBuilder.AddSubMenu(LabelText, ToolTipText, FNewMenuDelegate::CreateLambda(MenuAction), FUIAction(), NAME_None, EUserInterfaceActionType::None);

    InMenuBuilder.EndSection();
}

void FCommentNodeTranslationUtility::AddLanguageSubMenus(FMenuBuilder& InMenuBuilder, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
    const TArray<FString> EditorLanguages = GetEditorLanguages();

    for (const FString& EditorLanguage : EditorLanguages)
    {
        FInternationalization& Internationalization = FInternationalization::Get();
        FCulturePtr Culture = Internationalization.GetCulture(EditorLanguage);
        
        if (!Culture.IsValid())
        {
            continue;
        }

        const TAttribute LabelText = FText::FromString(Culture->GetEnglishName());
        const TAttribute ToolTipText = FText::FromString(TEXT("Translate comment of selected comment node to ") + Culture->GetEnglishName() + TEXT("."));

        const TSharedPtr<const FString> TranslationTargetLanguage = MakeShared<FString>(EditorLanguage);
        
        InMenuBuilder.AddMenuEntry(LabelText, ToolTipText, FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([InCommentNode, TranslationTargetLanguage]()
        {
            TranslateCommentNode(InCommentNode, TranslationTargetLanguage);
        })));
    }
}

TArray<FString> FCommentNodeTranslationUtility::GetEditorLanguages()
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

void FCommentNodeTranslationUtility::TranslateCommentNode(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode, const TSharedPtr<const FString> TranslationTargetLanguage)
{
	
}

FString FCommentNodeTranslationUtility::GetDeeplJsonRequest(const FString& SourceText, const FString& TargetLanguage)
{
    const TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    const TArray<TSharedPtr<FJsonValue>> SourceTexts = { MakeShared<FJsonValueString>(SourceText) };
    
    JsonObject->SetArrayField(TEXT("text"), SourceTexts);
    JsonObject->SetStringField(TEXT("target_lang"), TargetLanguage.ToUpper());

    FString DeeplJsonRequest;
    
    const TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&DeeplJsonRequest);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);
    
    return DeeplJsonRequest;
}

#undef LOCTEXT_NAMESPACE
