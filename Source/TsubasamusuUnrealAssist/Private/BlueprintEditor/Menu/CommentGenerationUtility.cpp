// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "BlueprintEditor/Menu/CommentGeneration/CommentGenerationUtility.h"

#define LOCTEXT_NAMESPACE "CommentGenerationUtility"

void FCommentGenerationUtility::AddCommentGenerationMenu(FMenuBuilder& InMenuBuilder, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
    const TAttribute LabelText = LOCTEXT("CommentGenerationLabelText", "Generate Comment");
    const TAttribute ToolTipText = LOCTEXT("CommentTranslationToolTipText", "Generate a comment based on the nodes contained in the comment node.");

    InMenuBuilder.AddMenuEntry(LabelText, ToolTipText, FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([InCommentNode]()
    {
    	if (InCommentNode.IsValid())
    	{
    		GenerateComment(InCommentNode);
    	}
    })));
	
    InMenuBuilder.EndSection();
}

void FCommentGenerationUtility::GenerateComment(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
	
}

#undef LOCTEXT_NAMESPACE
