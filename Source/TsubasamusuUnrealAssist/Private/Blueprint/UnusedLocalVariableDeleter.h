// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class FUnusedLocalVariableDeleter final
{
public:
	static void RegisterDeleteUnusedLocalVariablesMenu(UBlueprint* InBlueprint);
	
private:
	static void OnDeleteUnusedLocalVariablesClicked(UBlueprint* InBlueprint);
};
