// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "TsubasamusuNodeFactory.h"
#include "BlueprintEditor/BlueprintSuggesting/BlueprintSuggester.h"
#include "EdGraphNode_Comment.h"
#include "Widget/CommentNode/SGraphNodeGamingComment.h"
#include "Widget/CommentNode/SGraphNodePongComment.h"
#include "Setting/TsubasamusuUnrealAssistSettings.h"

TSharedPtr<SGraphNode> FTsubasamusuNodeFactory::CreateNode(UEdGraphNode* Node) const
{
	TSharedPtr<SGraphNode> ReturnNodeWidget = nullptr;

	if (!IsValid(Node))
	{
		return ReturnNodeWidget;
	}
	
	const UTsubasamusuUnrealAssistSettings* Settings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();
	UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(Node);
		
	if (IsValid(CommentNode))
	{
		switch (Settings->CommentNodeType)
		{
		case ECommentNodeType::Gaming:
			ReturnNodeWidget = SNew(SGraphNodeGamingComment, CommentNode);
			break;
		case ECommentNodeType::Pong:
			ReturnNodeWidget = SNew(SGraphNodePongComment, CommentNode);
			break;
		default:
			break;
		}
	}

	if (Settings->bEnableBlueprintSuggestion)
	{
		if (!BlueprintSuggester.IsValid())
		{
			BlueprintSuggester = MakeShared<FBlueprintSuggester>();
		}
	
		BlueprintSuggester->OnNodeAdded(Node);
	}

	return ReturnNodeWidget;
}
