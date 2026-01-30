// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class FGraphNodeUtility final
{
public:
	static TArray<UEdGraphNode*> GetSelectedNodes(const UEdGraph* InGraph);
	
	static bool TryGetNodesOutputPinsCommonType(const TArray<UEdGraphNode*>& InNodes, FEdGraphPinType& OutNodesOutputPinsCommonType);
	static bool NodeHasSameTypeOutputPins(const UEdGraphNode* InNode, const FEdGraphPinType& InPinType);
	static bool IsExecPinType(const FEdGraphPinType& InPinType);

	static TArray<UEdGraphPin*> GetNodesOutputPins(const TArray<UEdGraphNode*>& InNodes);
	static TArray<UEdGraphPin*> GetNodesOutputPins(const TArray<UEdGraphNode*>& InNodes, const FEdGraphPinType& SpecificPinType);
	static TArray<UEdGraphPin*> GetNodeInputPins(const UEdGraphNode* InNode);

	static void SortPinsByPositionY(TArray<UEdGraphPin*>& OutPins);
	static FIntPoint GetPinPosition(const UEdGraphPin* InPin);

	static TArray<UEdGraphNode*> ConvertToHardNodes(const TArray<TWeakObjectPtr<UEdGraphNode>>& InWeakNodes);
	static void RemoveInvalidWeakNodes(TArray<TWeakObjectPtr<UEdGraphNode>>& OutWeakNodes);
};
