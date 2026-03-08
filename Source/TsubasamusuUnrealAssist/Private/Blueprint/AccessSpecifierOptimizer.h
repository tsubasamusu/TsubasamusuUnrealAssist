// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "Type/TsubasamusuUnrealAssistMacros.h"
#include "BlueprintEditor.h"
#include "CoreMinimal.h"
#include "BlueprintMemberUtility.h"

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
#if EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
	static TMap<UFunction*, UK2Node_CustomEvent*> GetEvents(UBlueprint* InBlueprint);
#endif
	
	static TArray<TObjectPtr<const UBlueprint>> GetReferencerBlueprints(const UBlueprint* InReferencedBlueprint);
	
	template<typename FunctionToFindGraph, typename FunctionToCheckEditablePinNode, typename EntryNodeType>
	static TMap<UFunction*, EntryNodeType*> GetFunctionBaseMembers(UBlueprint* InBlueprint, const FunctionToFindGraph& InFunctionToFindGraph, const FunctionToCheckEditablePinNode& InFunctionToCheckEditablePinNode)
	{
		static_assert(TIsDerivedFrom<EntryNodeType, UK2Node_EditablePinBase>::Value, "EntryNodeType must be derived from UK2Node_EditablePinBase.");
		
		TMap<UFunction*, EntryNodeType*> FunctionBaseMembers;
	
		auto FunctionForEachFunctionBaseMembers = [&FunctionBaseMembers](UFunction* InFunction, UEdGraph* /*InGraph*/, UK2Node_EditablePinBase* InEditablePinNode)
		{
			EntryNodeType* EntryNode = Cast<EntryNodeType>(InEditablePinNode);
			
			if (IsValid(EntryNode))
			{
				FunctionBaseMembers.Add(InFunction, EntryNode);
			}
		};
		
		FBlueprintMemberUtility::ForEachFunctionBaseMembers(InBlueprint, InFunctionToFindGraph, FunctionForEachFunctionBaseMembers, InFunctionToCheckEditablePinNode);
		return FunctionBaseMembers;
	}
};
