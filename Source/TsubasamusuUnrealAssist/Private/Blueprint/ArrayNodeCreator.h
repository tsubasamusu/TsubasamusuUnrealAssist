// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class UK2Node_MakeArray;

class FArrayNodeCreator final
{
public:
	static void AddCreateArrayNodeMenu(const TWeakObjectPtr<UEdGraph> InGraph, FMenuBuilder& InMenuBuilder, const TSharedPtr<const FEdGraphPinType> InArrayNodePinType);
	static UK2Node_MakeArray* CreateArrayNode(const TWeakObjectPtr<UEdGraph> InGraph, const TArray<UEdGraphNode*>& InNodes, const FEdGraphPinType& InArrayNodePinType);

private:
	static FIntPoint GetDesiredArrayNodePosition(const TArray<UEdGraphNode*>& InNodes);

	static int32 GetNodesMaxPositionX(const TArray<UEdGraphNode*>& InNodes);
	static int32 GetNodesAveragePositionY(const TArray<UEdGraphNode*>& InNodes);
};
