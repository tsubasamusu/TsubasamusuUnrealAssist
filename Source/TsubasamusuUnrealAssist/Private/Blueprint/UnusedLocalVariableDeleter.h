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
	static bool IsLocalVariableUsed(const FBPVariableDescription& InLocalVariable, const UEdGraph* InFunctionGraph);
	static UK2Node_FunctionEntry* FindFunctionEntryNode(const UEdGraph* InGraph);
	static void DeleteLocalVariable(const FBPVariableDescription& InLocalVariable, UBlueprint* InBlueprint, UK2Node_FunctionEntry* InFunctionEntryNode);
};
