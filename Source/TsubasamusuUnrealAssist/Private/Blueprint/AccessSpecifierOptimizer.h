// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "Type/TsubasamusuUnrealAssistMacros.h"
#include "CoreMinimal.h"

class UK2Node_FunctionEntry;
class UK2Node_CustomEvent;
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
};
