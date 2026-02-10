// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "TsubasamusuBlueprintEditorCommands.h"

#define LOCTEXT_NAMESPACE "TsubasamusuUnrealAssist"

void FTsubasamusuBlueprintEditorCommands::RegisterCommands()
{
	UI_COMMAND(ChangeMemberVariablesAccessSpecifierToPrivate, "Change Member Variables Access Specifier to Private", "Change the access specifier of member variables not referenced by other classes to private.", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
