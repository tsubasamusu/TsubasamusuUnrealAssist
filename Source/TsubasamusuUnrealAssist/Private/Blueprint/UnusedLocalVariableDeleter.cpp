// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "UnusedLocalVariableDeleter.h"
#include "BlueprintCommandContext.h"
#include "CommandUtility.h"
#include "Command/TsubasamusuBlueprintEditorCommands.h"
#include "K2Node_FunctionEntry.h"

void FUnusedLocalVariableDeleter::RegisterDeleteUnusedLocalVariablesMenu(UBlueprint* InBlueprint)
{
	TWeakPtr<FBlueprintEditor> WeakBlueprintEditor = FCommandUtility::GetBlueprintEditor(InBlueprint);
	
	const FCanExecuteAction IsFocusingFunctionGraph = FCanExecuteAction::CreateLambda([WeakBlueprintEditor]()
	{
		if (WeakBlueprintEditor.IsValid())
		{
			const TSharedPtr<FBlueprintEditor> PinnedBlueprintEditor = WeakBlueprintEditor.Pin();
			const UEdGraph* FocusedGraph = PinnedBlueprintEditor->GetFocusedGraph();
			
			if (IsValid(FocusedGraph))
			{
				TArray<UK2Node_FunctionEntry*> FunctionEntryNodes;
				FocusedGraph->GetNodesOfClass(FunctionEntryNodes);
			
				return !FunctionEntryNodes.IsEmpty();
			}
		}
		
		return false;
	});
	
	const TArray<FCanExecuteAction> AdditionalConditionsToExecuteAction = { IsFocusingFunctionGraph };
	
	const FBlueprintCommandContext BlueprintCommandContext(FTsubasamusuBlueprintEditorCommands::Get().DeleteUnusedLocalVariables,
		FExecuteAction::CreateStatic(&OnDeleteUnusedLocalVariablesClicked, InBlueprint), InBlueprint, AdditionalConditionsToExecuteAction);
	
	FCommandUtility::RegisterCommandInBlueprintEditMenu(BlueprintCommandContext);
}

void FUnusedLocalVariableDeleter::OnDeleteUnusedLocalVariablesClicked(UBlueprint* InBlueprint)
{
	
}
