// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "TsubasamusuNodeFactory.h"
#include "EdGraphNode_Comment.h"
#include "SGraphNodeCustomizableComment.h"

TSharedPtr<SGraphNode> FTsubasamusuNodeFactory::CreateNode(class UEdGraphNode* Node) const
{
    UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(Node);

    if (IsValid(CommentNode))
    {
        return SNew(SGraphNodeCustomizableComment, CommentNode);
    }
    
    return nullptr;
}
