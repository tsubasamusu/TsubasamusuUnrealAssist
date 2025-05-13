// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class UK2Node_CallFunction;

class TSUBASAMUSUUNREALASSIST_API FCreateArrayNodeUtility
{
public:
	
	static void RegisterSelectedNodeMenu();

private:
	
	static TSharedRef<FExtender> ExtendSelectedNodeMenu(TSharedRef<FUICommandList> CommandList, const UEdGraph* InGraph, const UEdGraphNode* InNode, const UEdGraphPin* InPin, bool bIsDebugging);
	static void AddCreateArrayNodeMenu(const UEdGraph* InGraph, FMenuBuilder& MenuBuilder, const TSharedPtr<FEdGraphPinType> ArrayNodePinType);

	static void CreateArrayNode(const UEdGraph* InGraph, const TSharedPtr<FEdGraphPinType> ArrayNodePinType);
	
	static TArray<UEdGraphNode*> GetSelectedNodes(const UEdGraph* InGraph);
	static TArray<UEdGraphPin*> GetNodesOutputPins(const TArray<UEdGraphNode*>& InNodes);
	
	static bool TryGetNodesOutputPinsCommonType(const TArray<UEdGraphNode*>& InNodes, FEdGraphPinType& OutNodesOutputPinsCommonType);
	static bool NodeHasSameTypeOutputPins(const UEdGraphNode* InNode, const FEdGraphPinType& InPinType);
};
