// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class UK2Node_FunctionEntry;

class FUnusedLocalVariableDeleter final
{
public:
	static void RegisterDeleteUnusedLocalVariablesMenu(UBlueprint* InBlueprint);
	
private:
	static void OnDeleteUnusedLocalVariablesClicked(UBlueprint* InBlueprint);
	static UK2Node_FunctionEntry* FindFunctionEntryNode(const UEdGraph* InGraph);
};
