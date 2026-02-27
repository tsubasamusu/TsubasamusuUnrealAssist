// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "UnusedFunctionsDeleter.h"
#include "BlueprintCommandContext.h"
#include "BlueprintEditorModes.h"
#include "BlueprintMemberUtility.h"
#include "CommandUtility.h"
#include "K2Node_CreateDelegate.h"
#include "Command/TsubasamusuBlueprintEditorCommands.h"
#include "Debug/EditorMessageUtility.h"
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
	if (!IsValid(InBlueprint))
	{
		return;
	}
	
	auto FunctionToFindGraph = [](const FName& InFunctionName, UBlueprint* InBlueprintToFindGraph) -> UEdGraph*
	{
		return FBlueprintMemberUtility::FindFunctionGraph(InFunctionName, InBlueprintToFindGraph);
	};
	
	int32 AllDeletableFunctionsCount = 0;
	TArray<UEdGraph*> UnusedFunctionGraphs;
	
	auto FunctionForEachFunctionBaseMembersAndGraphs = [InBlueprint, &AllDeletableFunctionsCount, &UnusedFunctionGraphs](UFunction* InFunction, UEdGraph* InGraph)
	{
		if (InGraph->bAllowDeletion)
		{
			AllDeletableFunctionsCount++;
		
			if (!FBlueprintEditorUtils::IsFunctionUsed(InBlueprint, InFunction->GetFName()))
			{
				UnusedFunctionGraphs.Add(InGraph);
			}
		}
	};
	
	FBlueprintMemberUtility::ForEachFunctionBaseMembersAndGraphs(InBlueprint, FunctionToFindGraph, FunctionForEachFunctionBaseMembersAndGraphs);
	
	if (AllDeletableFunctionsCount == 0)
	{
		const FText NotificationText = LOCTEXT("NoFunctionsToCheck", "No functions to check for.");
		FEditorMessageUtility::DisplaySimpleNotification(NotificationText);
		return;
	}
	
	if (UnusedFunctionGraphs.IsEmpty())
	{
		const FText NotificationText = LOCTEXT("NoUnusedFunctions", "There are no unused functions.");
		FEditorMessageUtility::DisplaySimpleNotification(NotificationText);
		return;
	}
	
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
