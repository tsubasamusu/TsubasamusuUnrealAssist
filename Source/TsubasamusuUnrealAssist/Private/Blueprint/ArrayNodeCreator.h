// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class UK2Node_MakeArray;

class FArrayNodeCreator final
{
public:
	static void AddCreateArrayNodeMenu(const TWeakObjectPtr<UEdGraph> InGraph, FMenuBuilder& InMenuBuilder, const TSharedPtr<const FEdGraphPinType> InArrayNodePinType);
	
private:
	static UK2Node_MakeArray* CreateArrayNode(const TWeakObjectPtr<UEdGraph> InGraph, const TArray<UEdGraphNode*>& InNodes, const FEdGraphPinType& InArrayNodePinType);

	static TArray<UEdGraphPin*> GetInputPins(const UEdGraphNode* InNode);
	
	static TArray<UEdGraphPin*> GetOutputPins(const TWeakObjectPtr<UEdGraphNode> InNode);
	static TArray<UEdGraphPin*> GetOutputPins(const TWeakObjectPtr<UEdGraphNode> InNode, const FEdGraphPinType& InSpecificPinType);
	
	static TArray<UEdGraphPin*> GatherOutputPins(const TArray<TWeakObjectPtr<UEdGraphNode>>& InNodes);
	static TArray<UEdGraphPin*> GatherOutputPins(const TArray<TWeakObjectPtr<UEdGraphNode>>& InNodes, const FEdGraphPinType& InSpecificPinType);

	static bool TryGetNodesOutputPinsSameType(const TArray<TWeakObjectPtr<UEdGraphNode>>& InNodes, FEdGraphPinType& OutNodesOutputPinsSameType);
	static bool IsExecutionPinType(const FEdGraphPinType& InPinType);
	
	static void SortPinsByPositionY(TArray<UEdGraphPin*>& OutPins);
	static FIntPoint GetPinPosition(const UEdGraphPin* InPin);
	static FIntPoint GetDesiredArrayNodePosition(const TArray<TWeakObjectPtr<UEdGraphNode>>& InNodes);
	static int32 GetNodesMaxPositionX(const TArray<TWeakObjectPtr<UEdGraphNode>>& InNodes);
	static int32 GetNodesAveragePositionY(const TArray<TWeakObjectPtr<UEdGraphNode>>& InNodes);
};
