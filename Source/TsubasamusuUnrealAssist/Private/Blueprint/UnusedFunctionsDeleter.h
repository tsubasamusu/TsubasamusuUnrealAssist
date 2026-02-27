// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class FBlueprintEditor;

class FUnusedFunctionsDeleter final
{
public:
	static void RegisterDeleteUnusedFunctionsMenu(UBlueprint* InBlueprint);
	
private:
	static void OnDeleteUnusedFunctionsClicked(UBlueprint* InBlueprint);
};
