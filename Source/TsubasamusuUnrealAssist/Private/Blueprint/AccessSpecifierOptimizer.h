// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class FBlueprintMember;

class FAccessSpecifierOptimizer final
{
public:
	static void RegisterOptimizeAccessSpecifiersMenu(UBlueprint* InBlueprint);
	
private:
	static void OnOptimizeAccessSpecifiersClicked(UBlueprint* InBlueprint);
	static TSharedPtr<TArray<TSharedPtr<FBlueprintMember>>> GetMembers(UBlueprint* InBlueprint);
	static TArray<TObjectPtr<const UBlueprint>> GetReferencerBlueprints(const UBlueprint* InReferencedBlueprint);
};
