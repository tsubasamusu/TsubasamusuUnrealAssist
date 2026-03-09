// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "UnusedFunctionDeleter.h"
#include "BlueprintCommandContext.h"
#include "BlueprintMemberUtility.h"
#include "BlueprintEditorUtility.h"
#include "K2Node_CreateDelegate.h"
#include "Command/TsubasamusuBlueprintEditorCommands.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "FUnusedFunctionsDeleter"

void FUnusedFunctionDeleter::RegisterDeleteUnusedFunctionsMenu(UBlueprint* InBlueprint)
{
	const FBlueprintCommandContext BlueprintCommandContext(FTsubasamusuBlueprintEditorCommands::Get().DeleteUnusedFunctions,
		FExecuteAction::CreateStatic(&OnDeleteUnusedFunctionsClicked, InBlueprint), InBlueprint);
	
	FBlueprintEditorUtility::RegisterCommandInBlueprintEditMenu(BlueprintCommandContext);
}

void FUnusedFunctionDeleter::OnDeleteUnusedFunctionsClicked(UBlueprint* InBlueprint)
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
	TArray<TSharedPtr<FUnusedItem_FunctionGraph>> UnusedFunctionGraphs;
	
	auto FunctionForEachFunctionBaseMembers = [InBlueprint, &AllDeletableFunctionsCount, &UnusedFunctionGraphs](UFunction* InFunction, UEdGraph* InGraph, UK2Node_EditablePinBase* /*InEditablePinNode*/)
	{
		if (InGraph->bAllowDeletion)
		{
			AllDeletableFunctionsCount++;
		
			if (!FBlueprintEditorUtils::IsFunctionUsed(InBlueprint, InFunction->GetFName()))
			{
				const TSharedPtr<FUnusedItem_FunctionGraph> UnusedFunctionGraph = MakeShared<FUnusedItem_FunctionGraph>(InGraph);
				UnusedFunctionGraphs.Add(UnusedFunctionGraph);
			}
		}
	};
	
	auto FunctionToCheckEditablePinNode = [](const FName& /*InFunctionOrEventName*/, const UK2Node_EditablePinBase* InEditablePinNode)
	{
		return FBlueprintMemberUtility::IsFunctionEntryNode(InEditablePinNode);
	};
	
	FBlueprintMemberUtility::ForEachFunctionBaseMembers(InBlueprint, FunctionToFindGraph, FunctionForEachFunctionBaseMembers, FunctionToCheckEditablePinNode);
	
	const TSharedPtr<FBlueprintEditor> BlueprintEditor = FBlueprintEditorUtility::GetBlueprintEditor(InBlueprint);
	
	auto FunctionToDeleteUnusedItem = [&UnusedFunctionGraphs, InBlueprint, BlueprintEditor](const int32 ItemIndex)
	{
		DeleteFunction(UnusedFunctionGraphs[ItemIndex]->FunctionGraph, InBlueprint, BlueprintEditor);
	};
	
	TArray<TSharedPtr<FUnusedItem>> UnusedItems;
	for (const TSharedPtr<FUnusedItem_FunctionGraph> UnusedFunctionGraph : UnusedFunctionGraphs)
	{
		UnusedItems.Add(UnusedFunctionGraph);
	}
	
	const FText ItemTypeText = LOCTEXT("ItemType", "functions");
	FBlueprintEditorUtility::HandleDeletingUnusedItem(AllDeletableFunctionsCount, ItemTypeText, UnusedItems, FunctionToDeleteUnusedItem);
}

void FUnusedFunctionDeleter::DeleteFunction(UEdGraph* InFunctionGraph, UBlueprint* InBlueprint, const TSharedPtr<FBlueprintEditor> InBlueprintEditor)
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
