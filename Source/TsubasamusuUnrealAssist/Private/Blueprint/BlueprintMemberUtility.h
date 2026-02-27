// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class FBlueprintMemberUtility final
{
public:
	static UEdGraph* FindFunctionGraph(const FName& InFunctionName, const UBlueprint* InBlueprint);
	
	template<typename FunctionToFindGraph, typename FunctionToExecute>
	static void ForEachFunctionBaseMembersAndGraphs(UBlueprint* InBlueprint, const FunctionToFindGraph& InFunctionToFindGraph, const FunctionToExecute& InFunctionToExecute)
	{
		for (TFieldIterator<UFunction> FunctionFieldIterator(InBlueprint->SkeletonGeneratedClass, EFieldIterationFlags::None); FunctionFieldIterator; ++FunctionFieldIterator)
		{
			UFunction* Function = *FunctionFieldIterator;
		
			if (IsValid(Function))
			{
				const FName FunctionName = Function->GetFName();
				UEdGraph* Graph = InFunctionToFindGraph(FunctionName, InBlueprint);
			
				if (IsValid(Graph))
				{
					InFunctionToExecute(Function, Graph);
				}
			}
		}
	}
};
