// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "TsubasamusuBlueprintEditorCommands.h"

#define LOCTEXT_NAMESPACE "FTsubasamusuBlueprintEditorCommands"

void FTsubasamusuBlueprintEditorCommands::RegisterCommands()
{
	//TODO: When extending AccessSpecifierOptimizer beyond variables, also update the tooltip text here.
	UI_COMMAND(OptimizeAccessSpecifiers, "Optimize Access Specifiers", "Change the access specifiers of the variables defined in this blueprint to the appropriate ones.", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
