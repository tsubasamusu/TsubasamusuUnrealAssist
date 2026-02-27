// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "UnusedFunctionsDeleter.h"
#include "BlueprintCommandContext.h"
#include "BlueprintEditorModes.h"
#include "CommandUtility.h"
#include "K2Node_CreateDelegate.h"
#include "Command/TsubasamusuBlueprintEditorCommands.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "FUnusedFunctionsDeleter"

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

void FUnusedFunctionsDeleter::DeleteFunction(UEdGraph* InFunctionGraph, UBlueprint* InBlueprint, const TSharedPtr<FBlueprintEditor> InBlueprintEditor)
{
	if (!IsValid(InFunctionGraph) || !IsValid(InBlueprint) || !InFunctionGraph->bAllowDeletion || !InBlueprintEditor.IsValid())
	{
		return;;
	}
	
	if (const UEdGraphSchema* Schema = InFunctionGraph->GetSchema())
	{
		if (Schema->TryDeleteGraph(InFunctionGraph))
		{
			return;
		}
	}

	const FScopedTransaction Transaction(LOCTEXT("DeleteFunctionTransaction", "Delete Function"));
	
	InBlueprint->Modify();
	InFunctionGraph->Modify();

	FBlueprintEditorUtils::RemoveGraph(InBlueprint, InFunctionGraph, EGraphRemoveFlags::Recompile);
	InBlueprintEditor->CloseDocumentTab(InFunctionGraph);

	for (TObjectIterator<UK2Node_CreateDelegate> ObjectIterator(RF_ClassDefaultObject, true, EInternalObjectFlags::Garbage); ObjectIterator; ++ObjectIterator)
	{
		if (ObjectIterator->GetGraph() != InFunctionGraph)
		{
			if (IsValid(*ObjectIterator) && IsValid(ObjectIterator->GetGraph()))
			{
				ObjectIterator->HandleAnyChange();
			}
		}
	}

	InFunctionGraph = nullptr;
}

#undef LOCTEXT_NAMESPACE
