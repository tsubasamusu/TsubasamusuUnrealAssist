// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "UnusedFunctionsDeleter.h"
#include "BlueprintCommandContext.h"
#include "BlueprintEditorModes.h"
#include "CommandUtility.h"
#include "Command/TsubasamusuBlueprintEditorCommands.h"

void FUnusedFunctionsDeleter::RegisterDeleteUnusedFunctionsMenu(UBlueprint* InBlueprint)
{
	FTsubasamusuBlueprintEditorCommands::Register();

	const TArray<FName> TargetModes =
	{
		FBlueprintEditorApplicationModes::StandardBlueprintEditorMode,
		FBlueprintEditorApplicationModes::BlueprintInterfaceMode
	};

	const FBlueprintCommandContext BlueprintCommandContext(FTsubasamusuBlueprintEditorCommands::Get().DeleteUnusedFunctions,
		FExecuteAction::CreateStatic(&OnDeleteUnusedFunctionsClicked, InBlueprint),
		InBlueprint, TargetModes);
	
	FCommandUtility::RegisterCommandInBlueprintEditMenu(BlueprintCommandContext);
}

void FUnusedFunctionsDeleter::OnDeleteUnusedFunctionsClicked(UBlueprint* InBlueprint)
{
	
}
