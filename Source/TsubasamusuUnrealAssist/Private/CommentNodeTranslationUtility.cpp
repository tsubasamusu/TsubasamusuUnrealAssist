// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "CommentNodeTranslationUtility.h"
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

void FCommentNodeTranslationUtility::TranslateCommentNode(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
	
}

#undef LOCTEXT_NAMESPACE
