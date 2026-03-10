// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Type/TsubasamusuUnrealAssistEnums.h"

class FBlueprintMemberUtility final
{
public:
	static UEdGraph* FindFunctionGraph(const FName& InFunctionName, const UBlueprint* InBlueprint);
	static bool IsFunctionEntryNode(const UK2Node_EditablePinBase* InEditablePinNode);
	
	static void SetVariableAccessSpecifier(const ETsubasamusuAccessSpecifier InAccessSpecifier, const FName& InVariableName, UBlueprint* InBlueprint);
	
	template<typename FunctionToFindGraph, typename FunctionToExecute, typename  FunctionToCheckEditablePinNode>
	static void ForEachFunctionBaseMembers(UBlueprint* InBlueprint, const FunctionToFindGraph& InFunctionToFindGraph, const FunctionToExecute& InFunctionToExecute, const FunctionToCheckEditablePinNode& InFunctionToCheckEditablePinNode)
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
					UK2Node_EditablePinBase* EditablePinNode = FindEditablePinNode(Graph, FunctionName, InFunctionToCheckEditablePinNode);
					
					if (IsValid(EditablePinNode) && !FBlueprintEditorUtils::IsInterfaceBlueprint(EditablePinNode->GetBlueprint()) && EditablePinNode->IsEditable())
					{
						InFunctionToExecute(Function, Graph, EditablePinNode);
					}
				}
			}
		}
	}
	
	template<typename FunctionToCheckEditablePinNode>
	static UK2Node_EditablePinBase* FindEditablePinNode(const UEdGraph* InGraph, const FName& InFunctionOrEventName, const FunctionToCheckEditablePinNode& InFunctionToCheckEditablePinNode)
	{
		if (IsValid(InGraph))
		{
			TArray<UK2Node_EditablePinBase*> EditablePinNodes;
			InGraph->GetNodesOfClass(EditablePinNodes);
	
			if (!EditablePinNodes.IsEmpty())
			{
				for (UK2Node_EditablePinBase* EditablePinNode : EditablePinNodes)
				{
					if (IsValid(EditablePinNode) && InFunctionToCheckEditablePinNode(InFunctionOrEventName, EditablePinNode))
					{
						return EditablePinNode;
					}
				}
			}
		}
	
		return nullptr;
	}
};
