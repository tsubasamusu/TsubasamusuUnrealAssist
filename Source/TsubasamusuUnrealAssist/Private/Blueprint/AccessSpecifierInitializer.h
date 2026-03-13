// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Type/TsubasamusuUnrealAssistStructs.h"

class FAccessSpecifierInitializer final
{
public:
	void RegisterBlueprint(UBlueprint* InBlueprint);
	void UnregisterBlueprint(UBlueprint* InBlueprint);
	void UnregisterAllBlueprints();

private:
	void OnBlueprintChanged(UBlueprint* InBlueprint);
	static FBlueprintMemberSet CreateBlueprintMemberSet(UBlueprint* InBlueprint);
	void UnregisterBlueprintMemberSet(const FBlueprintMemberSet& InBlueprintMemberSet);
	
	bool IsRegisteredBlueprint(UBlueprint* InBlueprint);
	FBlueprintMemberSet FindBlueprintMemberSet(UBlueprint* InBlueprint);
	
	TArray<FBlueprintMemberSet> BlueprintMemberSets;
};
