// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "Type/TsubasamusuUnrealAssistEnums.h"
#include "BlueprintEditor.h"
#include "CoreMinimal.h"

class FOptimizeAccessSpecifiersUtility final
{
public:
	static void OnBlueprintEditorOpened(UBlueprint* InOpenedBlueprint);
	
private:
	static void RegisterAdditionalMenus(const TSharedPtr<FBlueprintEditor> InBlueprintEditor);
	static void OnOptimizeAccessSpecifiersClicked(const TSharedPtr<FBlueprintEditor> InBlueprintEditor);
	
	static TArray<FProperty*> GetVariables(const UBlueprint* InBlueprint);
	static void RemoveVariablesShouldNotBePrivate(TArray<FProperty*>& OutVariables, const UBlueprint* VariablesOwnerBlueprint);

	static TsubasamusuUnrealAssist::EAccessSpecifier GetOptimalAccessSpecifier(const FProperty* InVariable, const UBlueprint* VariableOwnerBlueprint);
	static TsubasamusuUnrealAssist::EAccessSpecifier GetCurrentAccessSpecifier(const FProperty* InVariable, const UBlueprint* VariableOwnerBlueprint);
	
	static TArray<UBlueprint*> GetBlueprintsReferenceVariable(const FProperty* InVariable, const UBlueprint* VariableOwnerBlueprint, const bool bExcludeVariableOwnerBlueprint = true);
	static TArray<UBlueprint*> GetReferencerBlueprints(const UBlueprint* InReferencedBlueprint);
	
	static bool IsBlueprintReferencesVariable(const UBlueprint* BlueprintToCheck, const FProperty* VariableToCheck, const UBlueprint* VariableOwnerBlueprint);
};
