// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "BlueprintEditor.h"
#include "CoreMinimal.h"

class FBlueprintMember;

class FAccessSpecifierOptimizer final
{
public:
	static void OnBlueprintEditorOpened(UBlueprint* InOpenedBlueprint);
	
private:
	static void RegisterAdditionalMenus(const TSharedPtr<FBlueprintEditor> InBlueprintEditor);
	static void OnOptimizeAccessSpecifiersClicked(const TSharedPtr<FBlueprintEditor> InBlueprintEditor);
	
	static TSharedPtr<TArray<TSharedPtr<FBlueprintMember>>> GetMembers(UBlueprint* InBlueprint);
	static TArray<FProperty*> GetVariables(const UBlueprint* InBlueprint);
	
	static TArray<TObjectPtr<const UBlueprint>> GetReferencerBlueprints(const UBlueprint* InReferencedBlueprint);
};
