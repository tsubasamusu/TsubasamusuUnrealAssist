// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Type/TsubasamusuUnrealAssistEnums.h"
#include "Type/TsubasamusuUnrealAssistMacros.h"

class UK2Node_FunctionEntry;
#if CUSTOM_EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
class UK2Node_CustomEvent;
#endif

class FBlueprintMemberUtility final
{
public:
	static UEdGraph* FindFunctionGraph(const FName& InFunctionName, const UBlueprint* InBlueprint);
	static bool IsFunctionEntryNode(const UK2Node_EditablePinBase* InEditablePinNode);
	
	static void SetVariableAccessSpecifier(const ETsubasamusuAccessSpecifier InAccessSpecifier, const FName& InVariableName, UBlueprint* InBlueprint);
	static void SetFunctionAccessSpecifier(const ETsubasamusuAccessSpecifier InAccessSpecifier, UFunction* InFunction, UK2Node_FunctionEntry* InFunctionEntryNode, UBlueprint* InBlueprint);
#if CUSTOM_EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
	static void SetCustomEventAccessSpecifier(const ETsubasamusuAccessSpecifier InAccessSpecifier, UFunction* InFunction, UK2Node_CustomEvent* InCustomEventEntryNode, UBlueprint* InBlueprint);
#endif
	
	static TArray<FProperty*> GetVariables(const UBlueprint* InBlueprint);
	static TMap<UFunction*, UK2Node_FunctionEntry*> GetFunctions(UBlueprint* InBlueprint);
#if CUSTOM_EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
	static TMap<UFunction*, UK2Node_CustomEvent*> GetEvents(UBlueprint* InBlueprint);
#endif
	
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
