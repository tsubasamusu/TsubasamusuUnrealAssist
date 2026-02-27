// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "BlueprintEditor.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "CoreMinimal.h"

class FBlueprintMember;

class FAccessSpecifierOptimizer final
{
public:
	static void RegisterOptimizeAccessSpecifiersMenu(UBlueprint* InBlueprint);
	
private:
	static void OnOptimizeAccessSpecifiersClicked(UBlueprint* InBlueprint);
	
	static TSharedPtr<TArray<TSharedPtr<FBlueprintMember>>> GetMembers(UBlueprint* InBlueprint);
	static TArray<FProperty*> GetVariables(const UBlueprint* InBlueprint);
	static TMap<UFunction*, UK2Node_FunctionEntry*> GetFunctions(UBlueprint* InBlueprint);
	static TMap<UFunction*, UK2Node_CustomEvent*> GetEvents(UBlueprint* InBlueprint);
	
	static TArray<TObjectPtr<const UBlueprint>> GetReferencerBlueprints(const UBlueprint* InReferencedBlueprint);
	
	template<typename FunctionToFindGraph, typename FunctionToCheckEditablePinNode, typename EntryNodeType>
	static TMap<UFunction*, EntryNodeType*> GetFunctionBaseMembers(UBlueprint* InBlueprint, const FunctionToFindGraph& InFunctionToFindGraph, const FunctionToCheckEditablePinNode& InFunctionToCheckEditablePinNode)
	{
		TMap<UFunction*, EntryNodeType*> FunctionBaseMembers;
	
		for (TFieldIterator<UFunction> FunctionFieldIterator(InBlueprint->SkeletonGeneratedClass, EFieldIterationFlags::None); FunctionFieldIterator; ++FunctionFieldIterator)
		{
			UFunction* Function = *FunctionFieldIterator;
		
			if (IsValid(Function))
			{
				const FName FunctionName = Function->GetFName();
				const UEdGraph* Graph = InFunctionToFindGraph(FunctionName, InBlueprint);
			
				if (IsValid(Graph))
				{
					EntryNodeType* EntryNode = FindEntryNode<EntryNodeType>(Graph, FunctionName, InFunctionToCheckEditablePinNode);
					
					if (IsValid(EntryNode) && !FBlueprintEditorUtils::IsInterfaceBlueprint(EntryNode->GetBlueprint()) && EntryNode->IsEditable())
					{
						FunctionBaseMembers.Add(Function, EntryNode);
					}
				}
			}
		}
	
		return FunctionBaseMembers;
	}
	
	template<typename EntryNodeType, typename FunctionToCheckEditablePinNode>
	static EntryNodeType* FindEntryNode(const UEdGraph* InGraph, const FName& InFunctionOrEventName, const FunctionToCheckEditablePinNode& InFunctionToCheckEditablePinNode)
	{
		static_assert(TIsDerivedFrom<EntryNodeType, UK2Node_EditablePinBase>::Value, "EntryNodeType must be derived from UK2Node_EditablePinBase.");

		if (IsValid(InGraph))
		{
			TArray<UK2Node_EditablePinBase*> EditablePinNodes;
			InGraph->GetNodesOfClass(EditablePinNodes);
	
			if (!EditablePinNodes.IsEmpty())
			{
				for (UK2Node_EditablePinBase* EditablePinNode : EditablePinNodes)
				{
					if (InFunctionToCheckEditablePinNode(InFunctionOrEventName, EditablePinNode))
					{
						EntryNodeType* EntryNode = Cast<EntryNodeType>(EditablePinNode);
						
						if (IsValid(EntryNode))
						{
							return EntryNode;
						}
					}
				}
			}
		}
	
		return nullptr;
	}
};
