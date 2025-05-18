// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class TSUBASAMUSUUNREALASSIST_API FCreateArrayNodeUtility
{
public:
	
	static void AddCreateArrayNodeMenu(const UEdGraph* InGraph, FMenuBuilder& MenuBuilder, const TSharedPtr<FEdGraphPinType> ArrayNodePinType);

private:
	
	static void CreateArrayNode(UEdGraph* InGraph, const TSharedPtr<FEdGraphPinType> ArrayNodePinType);
	
	static FIntPoint GetDesiredArrayNodePosition(const UEdGraph* InGraph);

	static int32 GetNodesMaxPositionX(const TArray<UEdGraphNode*>& InNodes);
	static int32 GetNodesAveragePositionY(const TArray<UEdGraphNode*>& InNodes);
};
