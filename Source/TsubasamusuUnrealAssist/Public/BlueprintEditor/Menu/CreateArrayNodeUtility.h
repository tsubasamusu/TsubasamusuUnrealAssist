// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class UK2Node_MakeArray;

class TSUBASAMUSUUNREALASSIST_API FCreateArrayNodeUtility final
{
public:
	
	static void AddCreateArrayNodeMenu(const TWeakObjectPtr<UEdGraph> InGraph, FMenuBuilder& InMenuBuilder, const TSharedPtr<const FEdGraphPinType> ArrayNodePinType);
	static UK2Node_MakeArray* CreateArrayNode(const TWeakObjectPtr<UEdGraph> InGraph, const TArray<UEdGraphNode*>& InNodes, const FEdGraphPinType& ArrayNodePinType);

private:
	
	static FIntPoint GetDesiredArrayNodePosition(const TArray<UEdGraphNode*>& InNodes);

	static int32 GetNodesMaxPositionX(const TArray<UEdGraphNode*>& InNodes);
	static int32 GetNodesAveragePositionY(const TArray<UEdGraphNode*>& InNodes);
};
