// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "CommentNodeTranslationUtility.h"

#define LOCTEXT_NAMESPACE "FCommentNodeTranslationUtility"

void FCommentNodeTranslationUtility::AddCommentNodeTranslationMenu(FMenuBuilder& InMenuBuilder, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
    const FName ExtensionHookName = TEXT("TsubasamusuUnrealAssistSection");
    const TAttribute HeadingText = LOCTEXT("TsubasamusuUnrealAssistSectionHeader", "Tsubasamusu Unreal Assist");
    
	InMenuBuilder.BeginSection(ExtensionHookName, HeadingText);

    const TAttribute LabelText = LOCTEXT("CommentNodeTranslationLabelText", "Translate");
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

void FCommentNodeTranslationUtility::TranslateCommentNode(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
	
}

#undef LOCTEXT_NAMESPACE
