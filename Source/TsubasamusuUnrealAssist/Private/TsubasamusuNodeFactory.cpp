// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "TsubasamusuNodeFactory.h"
#include "BlueprintSuggester.h"
#include "CommentNodeType.h"
#include "EdGraphNode_Comment.h"
#include "Widget/CommentNode/SGraphNodeGamingComment.h"
#include "Widget/CommentNode/SGraphNodePongComment.h"
#include "TsubasamusuUnrealAssistSettings.h"

TSharedPtr<SGraphNode> FTsubasamusuNodeFactory::CreateNode(UEdGraphNode* Node) const
{
	TSharedPtr<SGraphNode> ReturnNodeWidget = nullptr;

	if (!IsValid(Node))
	{
		return ReturnNodeWidget;
	}
	
	UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(Node);

	if (IsValid(CommentNode))
	{
		const UTsubasamusuUnrealAssistSettings* Settings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();
		
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

	if (!BlueprintSuggester.IsValid())
	{
		BlueprintSuggester = MakeShared<FBlueprintSuggester>();
	}
	
	BlueprintSuggester->OnNodeAdded(Node);

	return ReturnNodeWidget;
}
