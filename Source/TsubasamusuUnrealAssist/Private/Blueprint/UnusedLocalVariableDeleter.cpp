// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "UnusedLocalVariableDeleter.h"
#include "BlueprintCommandContext.h"
#include "BlueprintEditorUtility.h"
#include "Command/TsubasamusuBlueprintEditorCommands.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_Variable.h"
#include "SCheckBoxList.h"
#include "Algo/AnyOf.h"
#include "Debug/EditorMessageUtility.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "FUnusedLocalVariableDeleter"

void FUnusedLocalVariableDeleter::RegisterDeleteUnusedLocalVariablesMenu(UBlueprint* InBlueprint)
{
	TWeakPtr<FBlueprintEditor> WeakBlueprintEditor = FBlueprintEditorUtility::GetBlueprintEditor(InBlueprint);
	
	const FCanExecuteAction IsFocusingFunctionGraph = FCanExecuteAction::CreateLambda([WeakBlueprintEditor]()
	{
		if (WeakBlueprintEditor.IsValid())
		{
			const TSharedPtr<FBlueprintEditor> PinnedBlueprintEditor = WeakBlueprintEditor.Pin();
			const UEdGraph* FocusedGraph = PinnedBlueprintEditor->GetFocusedGraph();
			
			if (IsValid(FocusedGraph))
			{
				const UK2Node_FunctionEntry* FunctionEntryNode = FindFunctionEntryNode(FocusedGraph);
				return IsValid(FunctionEntryNode);
			}
		}
		
		return false;
	});
	
	const TArray<FCanExecuteAction> AdditionalConditionsToExecuteAction = { IsFocusingFunctionGraph };
	
	const FBlueprintCommandContext BlueprintCommandContext(FTsubasamusuBlueprintEditorCommands::Get().DeleteUnusedLocalVariables,
		FExecuteAction::CreateStatic(&OnDeleteUnusedLocalVariablesClicked, InBlueprint), InBlueprint, AdditionalConditionsToExecuteAction);
	
	FBlueprintEditorUtility::RegisterCommandInBlueprintEditMenu(BlueprintCommandContext);
}

void FUnusedLocalVariableDeleter::OnDeleteUnusedLocalVariablesClicked(UBlueprint* InBlueprint)
{
	const TSharedPtr<FBlueprintEditor> BlueprintEditor = FBlueprintEditorUtility::GetBlueprintEditor(InBlueprint);
	const UEdGraph* FocusedGraph = BlueprintEditor->GetFocusedGraph();
	UK2Node_FunctionEntry* FunctionEntryNode = FindFunctionEntryNode(FocusedGraph);
	
	if (FunctionEntryNode->LocalVariables.IsEmpty())
	{
		const FText NotificationText = LOCTEXT("NoLocalVariablesToCheck", "No local variables to check for.");
		FEditorMessageUtility::DisplaySimpleNotification(NotificationText);
		return;
	}
	
	TArray<FBPVariableDescription> UnusedLocalVariables;
	
	for (const FBPVariableDescription& LocalVariable : FunctionEntryNode->LocalVariables)
	{
		if (!IsLocalVariableUsed(LocalVariable, FocusedGraph))
		{
			UnusedLocalVariables.Add(LocalVariable);
		}
	}
	
	if (UnusedLocalVariables.IsEmpty())
	{
		const FText NotificationText = LOCTEXT("NoUnusedLocalVariables", "There are no unused local variables.");
		FEditorMessageUtility::DisplaySimpleNotification(NotificationText);
		return;
	}

	const TSharedRef<SCheckBoxList> CheckBoxList = SNew(SCheckBoxList)
			.ItemHeaderLabel(LOCTEXT("DeleteUnusedLocalVariablesDialog_LocalVariableLabel", "Local Variable"));
	
	for (const FBPVariableDescription& UnusedLocalVariable : UnusedLocalVariables)
	{
		CheckBoxList->AddItem(FText::FromName(UnusedLocalVariable.VarName), true);
	}

	auto OneOrMoreLocalVariablesAreChecked = [&UnusedLocalVariables, CheckBoxList]()
	{
		for (int32 Index = 0; Index < UnusedLocalVariables.Num(); ++Index)
		{
			if (CheckBoxList->IsItemChecked(Index))
			{
				return true;
			}
		}
		
		return false;
	};
	
	const FText DialogTitle = LOCTEXT("DeleteUnusedLocalVariablesDialog_Title", "Delete Unused Local Variables");
	const FText DialogMessage = LOCTEXT("DeleteUnusedLocalVariablesDialog_Message", "These local variables are not used in the graph.");
	const FText ApplyButtonText = LOCTEXT("DeleteUnusedLocalVariablesDialog_ApplyButton", "Delete Selected Local Variables");
	const FText CancelButtonText = LOCTEXT("DeleteUnusedLocalVariablesDialog_CancelButton", "Cancel");

	const TsubasamusuUnrealAssist::EDialogButton PressedButton = FEditorMessageUtility::ShowCustomDialog(DialogTitle, DialogMessage, ApplyButtonText, CancelButtonText, CheckBoxList, TAttribute<bool>::CreateLambda(OneOrMoreLocalVariablesAreChecked));
	
	if (PressedButton != TsubasamusuUnrealAssist::EDialogButton::OK || !OneOrMoreLocalVariablesAreChecked())
	{
		return;
	}
	
	for (int32 Index = 0; Index < UnusedLocalVariables.Num(); ++Index)
	{
		if (CheckBoxList->IsItemChecked(Index))
		{
			DeleteLocalVariable(UnusedLocalVariables[Index], InBlueprint, FunctionEntryNode);
		}
	}

	const FText NotificationText = LOCTEXT("SuccessfullyDeleteUnusedLocalVariables", "Successfully delete unused local variables.");
	FEditorMessageUtility::DisplaySimpleNotification(NotificationText, SNotificationItem::ECompletionState::CS_Success);
}

bool FUnusedLocalVariableDeleter::IsLocalVariableUsed(const FBPVariableDescription& InLocalVariable, const UEdGraph* InFunctionGraph)
{
	TArray<UK2Node_Variable*> VariableNodes;
	InFunctionGraph->GetNodesOfClass(VariableNodes);
	
	auto IsVariableNodeReferencesLocalVariable = [InLocalVariable](const UK2Node_Variable* InVariableNode)
	{
		UClass* BlueprintClass = InVariableNode->GetBlueprintClassFromNode();
		const FProperty* Variable = InVariableNode->VariableReference.ResolveMember<FProperty>(BlueprintClass);
		const bool bIsLocalVariableNode = Variable && !Variable->HasAnyPropertyFlags(CPF_Parm) && InVariableNode->VariableReference.IsLocalScope();
		
		return bIsLocalVariableNode && InVariableNode->GetVarName() == InLocalVariable.VarName;
	};
	
	return Algo::AnyOf(VariableNodes, IsVariableNodeReferencesLocalVariable);
}

UK2Node_FunctionEntry* FUnusedLocalVariableDeleter::FindFunctionEntryNode(const UEdGraph* InGraph)
{
	if (IsValid(InGraph))
	{
		TArray<UK2Node_FunctionEntry*> FunctionEntryNodes;
		InGraph->GetNodesOfClass(FunctionEntryNodes);
	
		if (!FunctionEntryNodes.IsEmpty())
		{
			check(FunctionEntryNodes.Num() == 1);
			return FunctionEntryNodes[0];
		}
	}
	
	return nullptr;
}

void FUnusedLocalVariableDeleter::DeleteLocalVariable(const FBPVariableDescription& InLocalVariable, UBlueprint* InBlueprint, UK2Node_FunctionEntry* InFunctionEntryNode)
{
	if (IsValid(InBlueprint) && IsValid(InFunctionEntryNode))
	{
		const FScopedTransaction Transaction(LOCTEXT("DeleteLocalVariableTransaction", "Delete Local Variable"));

		InBlueprint->Modify();
		InFunctionEntryNode->Modify();

		const UStruct* Scope = CastChecked<UStruct>(InFunctionEntryNode->FindSignatureFunction());
		FBlueprintEditorUtils::RemoveLocalVariable(InBlueprint, Scope, InLocalVariable.VarName);
	}
}

#undef LOCTEXT_NAMESPACE
