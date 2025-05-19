// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "CommentNodeTranslationUtility.h"

#define LOCTEXT_NAMESPACE "FCommentNodeTranslationUtility"

void FCommentNodeTranslationUtility::AddCommentNodeTranslationMenu(FMenuBuilder& InMenuBuilder, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
	InMenuBuilder.BeginSection(TEXT("TsubasamusuUnrealAssistSection"), LOCTEXT("TsubasamusuUnrealAssistSectionHeader", "Tsubasamusu Unreal Assist"));
	
	const FText CreateArrayNodeLabelText = LOCTEXT("TranslateCommentNodeLabelText", "Translate");
	const FText CreateArrayNodeToolTipText = LOCTEXT("TranslateCommentNodeToolTipText", "Translate comment of selected comment node.");

	InMenuBuilder.AddMenuEntry(CreateArrayNodeLabelText, CreateArrayNodeToolTipText, FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([InCommentNode]()
	{
		TranslateCommentNode(InCommentNode);
	})));
	
	InMenuBuilder.EndSection();
}

void FCommentNodeTranslationUtility::TranslateCommentNode(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
	
}

#undef LOCTEXT_NAMESPACE
