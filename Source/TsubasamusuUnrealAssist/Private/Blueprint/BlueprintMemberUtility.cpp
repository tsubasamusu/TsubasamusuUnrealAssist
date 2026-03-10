// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "BlueprintMemberUtility.h"
#include "K2Node_FunctionEntry.h"

#define LOCTEXT_NAMESPACE "FBlueprintMemberUtility"

UEdGraph* FBlueprintMemberUtility::FindFunctionGraph(const FName& InFunctionName, const UBlueprint* InBlueprint)
{
	if (IsValid(InBlueprint))
	{
		for (const TObjectPtr<UEdGraph> FunctionGraph : InBlueprint->FunctionGraphs)
		{
			if (IsValid(FunctionGraph) && FunctionGraph->GetFName() == InFunctionName)
			{
				return FunctionGraph;
			}
		}
	}
		
	return nullptr;
}

bool FBlueprintMemberUtility::IsFunctionEntryNode(const UK2Node_EditablePinBase* InEditablePinNode)
{
	return InEditablePinNode->IsA<UK2Node_FunctionEntry>();
}

void FBlueprintMemberUtility::SetVariableAccessSpecifier(const ETsubasamusuAccessSpecifier InAccessSpecifier, const FName& InVariableName, UBlueprint* InBlueprint)
{
	if (IsValid(InBlueprint))
	{
		const FScopedTransaction Transaction(LOCTEXT("ChangeVariableAccessSpecifierTransaction", "Change Variable Access Specifier"));
		
		InBlueprint->Modify();
		
		switch (InAccessSpecifier)
		{
		case ETsubasamusuAccessSpecifier::Private:
			FBlueprintEditorUtils::SetBlueprintVariableMetaData(InBlueprint, InVariableName, nullptr, FBlueprintMetadata::MD_Private, TEXT("true"));
			break;
		case ETsubasamusuAccessSpecifier::Public:
			FBlueprintEditorUtils::RemoveBlueprintVariableMetaData(InBlueprint, InVariableName, nullptr, FBlueprintMetadata::MD_Private);
			break;
		default:
			checkNoEntry()
			break;
		}

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(InBlueprint);
	}
}

#undef LOCTEXT_NAMESPACE
