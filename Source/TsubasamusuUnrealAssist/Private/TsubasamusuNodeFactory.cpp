// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "TsubasamusuNodeFactory.h"
#include "CommentNodeType.h"
#include "EdGraphNode_Comment.h"
#include "Widget/CommentNode/SGraphNodeGamingComment.h"
#include "Widget/CommentNode/SGraphNodePongComment.h"
#include "TsubasamusuUnrealAssistSettings.h"

TSharedPtr<SGraphNode> FTsubasamusuNodeFactory::CreateNode(class UEdGraphNode* Node) const
{
	UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(Node);

	if (!IsValid(CommentNode))
	{
		return nullptr;
	}

	const UTsubasamusuUnrealAssistSettings* Settings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();

	switch (Settings->CommentNodeType)
	{
	case ECommentNodeType::Normal:
		return nullptr;
	case ECommentNodeType::Gaming:
		return SNew(SGraphNodeGamingComment, CommentNode);
	case ECommentNodeType::Pong:
		return SNew(SGraphNodePongComment, CommentNode);
	default:
		return nullptr;
	}
}
