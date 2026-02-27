// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "UnusedFunctionsDeleter.h"
#include "BlueprintCommandContext.h"
#include "BlueprintEditorModes.h"
#include "BlueprintMemberUtility.h"
#include "CommandUtility.h"
#include "K2Node_CreateDelegate.h"
#include "SCheckBoxList.h"
#include "Algo/AnyOf.h"
#include "Command/TsubasamusuBlueprintEditorCommands.h"
#include "Debug/EditorMessageUtility.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Toolkits/ToolkitManager.h"

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

	const TSharedRef<SCheckBoxList> CheckBoxList = SNew(SCheckBoxList)
			.ItemHeaderLabel(LOCTEXT("DeleteUnusedFunctionsDialog_FunctionLabel", "Function"));
	
	for (const UEdGraph* UnusedFunctionGraph : UnusedFunctionGraphs)
	{
		CheckBoxList->AddItem(FText::FromString(UnusedFunctionGraph->GetName()), true);
	}

	auto OneOrMoreFunctionsAreChecked = [&UnusedFunctionGraphs, CheckBoxList]()
	{
		for (int32 Index = 0; Index < UnusedFunctionGraphs.Num(); ++Index)
		{
			if (CheckBoxList->IsItemChecked(Index))
			{
				return true;
			}
		}
		
		return false;
	};
	
	const FText DialogTitle = LOCTEXT("DeleteUnusedFunctionsDialog_Title", "Delete Unused Functions");
	const FText DialogMessage = LOCTEXT("DeleteUnusedFunctionsDialog_Message", "These functions are not used in the graph or in other blueprints' graphs.\nThey may be used in other places.\nYou may use 'Find in Blueprint' or the 'Asset Search' to find out if they are referenced elsewhere.");
	const FText ApplyButtonText = LOCTEXT("DeleteUnusedFunctionsDialog_ApplyButton", "Delete Selected Functions");
	const FText CancelButtonText = LOCTEXT("DeleteUnusedFunctionsDialog_CancelButton", "Cancel");

	const TsubasamusuUnrealAssist::EDialogButton PressedButton = FEditorMessageUtility::ShowCustomDialog(DialogTitle, DialogMessage, ApplyButtonText, CancelButtonText, CheckBoxList, TAttribute<bool>::CreateLambda(OneOrMoreFunctionsAreChecked));
	
	if (PressedButton != TsubasamusuUnrealAssist::EDialogButton::OK || !OneOrMoreFunctionsAreChecked())
	{
		return;
	}
	
	const TSharedPtr<IToolkit> Toolkit = FToolkitManager::Get().FindEditorForAsset(InBlueprint);
	const TSharedPtr<FBlueprintEditor> BlueprintEditor = StaticCastSharedPtr<FBlueprintEditor>(Toolkit);
	
	for (int32 Index = 0; Index < UnusedFunctionGraphs.Num(); ++Index)
	{
		if (CheckBoxList->IsItemChecked(Index))
		{
			DeleteFunction(UnusedFunctionGraphs[Index], InBlueprint, BlueprintEditor);
		}
	}

	const FText NotificationText = LOCTEXT("SuccessfullyDeleteUnusedFunctions", "Successfully delete unused functions.");
	FEditorMessageUtility::DisplaySimpleNotification(NotificationText, SNotificationItem::ECompletionState::CS_Success);
}

void FUnusedFunctionsDeleter::DeleteFunction(UEdGraph* InFunctionGraph, UBlueprint* InBlueprint, const TSharedPtr<FBlueprintEditor> InBlueprintEditor)
{
	if (!IsValid(InFunctionGraph) || !IsValid(InBlueprint) || !InFunctionGraph->bAllowDeletion || !InBlueprintEditor.IsValid())
	{
		return;
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
