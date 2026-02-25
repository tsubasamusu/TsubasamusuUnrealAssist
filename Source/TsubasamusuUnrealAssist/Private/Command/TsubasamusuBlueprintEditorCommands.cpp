// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "TsubasamusuBlueprintEditorCommands.h"

#define LOCTEXT_NAMESPACE "FTsubasamusuBlueprintEditorCommands"

void FTsubasamusuBlueprintEditorCommands::RegisterCommands()
{
	UI_COMMAND(OptimizeAccessSpecifiers, "Optimize Access Specifiers", "Change the access specifiers of the members defined in this blueprint to the optimal ones", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(DeleteUnusedFunctions, "Delete Unused Functions", "Delete any functions that are never used", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
