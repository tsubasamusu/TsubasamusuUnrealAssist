// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class TSUBASAMUSUUNREALASSIST_API FGraphNodeUtility
{
public:
	
	static TArray<UEdGraphNode*> GetSelectedNodes(const UEdGraph* InGraph);
	
	static bool TryGetNodesOutputPinsCommonType(const TArray<UEdGraphNode*>& InNodes, FEdGraphPinType& OutNodesOutputPinsCommonType);
	static bool NodeHasSameTypeOutputPins(const UEdGraphNode* InNode, const FEdGraphPinType& InPinType);

	static TArray<UEdGraphPin*> GetNodesOutputPins(const TArray<UEdGraphNode*>& InNodes);
	static TArray<UEdGraphPin*> GetNodesOutputPins(const TArray<UEdGraphNode*>& InNodes, const FEdGraphPinType& SpecificPinType);
	static TArray<UEdGraphPin*> GetNodeInputPins(const UEdGraphNode* InNode);

	static void SortPinsByPositionY(TArray<UEdGraphPin*>& OutPins);
	static FIntPoint GetPinPosition(const UEdGraphPin* InPin);
};
