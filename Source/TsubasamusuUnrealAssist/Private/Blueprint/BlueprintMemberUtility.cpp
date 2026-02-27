// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "BlueprintMemberUtility.h"

UEdGraph* FBlueprintMemberUtility::FindFunctionGraph(const FName& InFunctionName, const UBlueprint* InBlueprint)
{
	if (IsValid(InBlueprint))
	{
		for (const TObjectPtr<UEdGraph> FunctionGraph : InBlueprint->FunctionGraphs)
		{
			if (IsValid(FunctionGraph) && FunctionGraph->GetFName() == InFunctionName)
			{
				return FunctionGraph;
			}
		}
	}
		
	return nullptr;
}
