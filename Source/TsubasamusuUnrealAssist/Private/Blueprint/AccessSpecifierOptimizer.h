// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "Type/TsubasamusuUnrealAssistEnums.h"
#include "BlueprintEditor.h"
#include "CoreMinimal.h"

class FAccessSpecifierOptimizer final
{
public:
	static void OnBlueprintEditorOpened(UBlueprint* InOpenedBlueprint);
	
private:
	static void RegisterAdditionalMenus(const TSharedPtr<FBlueprintEditor> InBlueprintEditor);
	static void OnOptimizeAccessSpecifiersClicked(const TSharedPtr<FBlueprintEditor> InBlueprintEditor);
	
	static TArray<FProperty*> GetVariables(const UBlueprint* InBlueprint);
	static void RemoveVariablesShouldNotBePrivate(TArray<FProperty*>& OutVariables, const UBlueprint* InVariablesOwnerBlueprint);

	static TsubasamusuUnrealAssist::EAccessSpecifier GetOptimalAccessSpecifier(const FProperty* InVariable, const UBlueprint* InVariableOwnerBlueprint);
	static TsubasamusuUnrealAssist::EAccessSpecifier GetCurrentAccessSpecifier(const FProperty* InVariable, const UBlueprint* InVariableOwnerBlueprint);
	
	static TArray<UBlueprint*> GetBlueprintsReferenceVariable(const FProperty* InVariable, const UBlueprint* InVariableOwnerBlueprint, const bool bInExcludeVariableOwnerBlueprint = true);
	static TArray<UBlueprint*> GetReferencerBlueprints(const UBlueprint* InReferencedBlueprint);
	
	static bool IsBlueprintReferencesVariable(const UBlueprint* InBlueprintToCheck, const FProperty* InVariableToCheck, const UBlueprint* InVariableOwnerBlueprint);
};
