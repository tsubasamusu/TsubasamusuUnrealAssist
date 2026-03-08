// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "BlueprintMemberUtility.h"
#include "K2Node_FunctionEntry.h"

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

bool FBlueprintMemberUtility::IsFunctionEntryNode(const UK2Node_EditablePinBase* InEditablePinNode)
{
	return InEditablePinNode->IsA<UK2Node_FunctionEntry>();
}
