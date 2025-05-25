// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class TSUBASAMUSUUNREALASSIST_API FCreateArrayNodeUtility final
{
public:
	
	static void AddCreateArrayNodeMenu(const TWeakObjectPtr<UEdGraph> InGraph, FMenuBuilder& InMenuBuilder, const TSharedPtr<const FEdGraphPinType> ArrayNodePinType);

private:
	
	static void CreateArrayNode(const TWeakObjectPtr<UEdGraph> InGraph, const TSharedPtr<const FEdGraphPinType> ArrayNodePinType);
	
	static FIntPoint GetDesiredArrayNodePosition(const TArray<UEdGraphNode*>& InNodes);

	static int32 GetNodesMaxPositionX(const TArray<UEdGraphNode*>& InNodes);
	static int32 GetNodesAveragePositionY(const TArray<UEdGraphNode*>& InNodes);
};
