// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "PlayerCharacterNodeFactory.h"
#include "EdGraphNode_Comment.h"
#include "SGraphNodePlayerCharacter.h"

TSharedPtr<SGraphNode> FPlayerCharacterNodeFactory::CreateNode(class UEdGraphNode* Node) const
{
    UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(Node);

    if (IsValid(CommentNode))
    {
        return SNew(SGraphNodePlayerCharacter, CommentNode);
    }
    
    return nullptr;
}
