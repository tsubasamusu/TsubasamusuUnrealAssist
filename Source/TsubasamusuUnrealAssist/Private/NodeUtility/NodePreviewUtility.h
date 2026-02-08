// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

struct FGraphActionNode;

class FNodePreviewUtility final
{
public:
	static TSharedPtr<SWidget> GetHoveredWidget();
	static bool IsNodeSelectionWidget(const TSharedPtr<SWidget> InWidget);
	static TSharedPtr<FGraphActionNode> GetGraphActionNodeFromWidget(const TSharedPtr<SWidget> InWidget);
	static TSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>> GetNodeTreeViewFromWidget(const TSharedPtr<SWidget> InWidget);
	static UEdGraphNode* CreateNodeFromGraphActionNode(const TSharedPtr<FGraphActionNode> InGraphActionNode);
	static TSharedPtr<SGraphNode> CreateNodeWidget(UEdGraphNode* InNode);
};
