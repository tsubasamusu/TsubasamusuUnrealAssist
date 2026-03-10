// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "BlueprintMemberUtility.h"
#include "K2Node_FunctionEntry.h"
#include "Type/TsubasamusuUnrealAssistMacros.h"

#if EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
#include "K2Node_CustomEvent.h"
#endif

#define LOCTEXT_NAMESPACE "FBlueprintMemberUtility"

template<typename FunctionToSetEntryNodeAccessSpecifier>
void SetFunctionBaseMemberAccessSpecifier(const ETsubasamusuAccessSpecifier InAccessSpecifier, UFunction* InFunction, UK2Node_EditablePinBase* InEntryNode, UBlueprint* InBlueprint, const FunctionToSetEntryNodeAccessSpecifier& InFunctionToSetEntryNodeAccessSpecifier)
{
	if(IsValid(InFunction) && IsValid(InEntryNode) && IsValid(InBlueprint))
	{
		const FScopedTransaction Transaction(LOCTEXT("ChangeFunctionBaseMemberAccessSpecifierTransaction", "Change Function Base Member Access Specifier"));

		InEntryNode->Modify();
		InFunction->Modify();

		EFunctionFlags AccessSpecifierFlag;
		switch (InAccessSpecifier)
		{
		case ETsubasamusuAccessSpecifier::Private:
			AccessSpecifierFlag = FUNC_Private;
			break;
		case ETsubasamusuAccessSpecifier::Protected:
			AccessSpecifierFlag = FUNC_Protected;
			break;
		case ETsubasamusuAccessSpecifier::Public:
			AccessSpecifierFlag = FUNC_Public;
			break;
		default:
			checkNoEntry();
			return;
		}

		constexpr EFunctionFlags ClearAccessSpecifierMask = ~FUNC_AccessSpecifiers;
		InFunctionToSetEntryNodeAccessSpecifier(AccessSpecifierFlag, ClearAccessSpecifierMask);
	
		InFunction->FunctionFlags &= ClearAccessSpecifierMask;
		InFunction->FunctionFlags |= AccessSpecifierFlag;

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(InBlueprint);
	}
}

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

void FBlueprintMemberUtility::SetFunctionAccessSpecifier(const ETsubasamusuAccessSpecifier InAccessSpecifier, UFunction* InFunction, UK2Node_FunctionEntry* InFunctionEntryNode, UBlueprint* InBlueprint)
{
	auto FunctionToSetEntryNodeAccessSpecifier = [InFunctionEntryNode](const EFunctionFlags InAccessSpecifierFlag, const EFunctionFlags InClearAccessSpecifierMask)
	{
		int32 ExtraFlags = InFunctionEntryNode->GetExtraFlags();
		ExtraFlags &= InClearAccessSpecifierMask;
		ExtraFlags |= InAccessSpecifierFlag;
		InFunctionEntryNode->SetExtraFlags(ExtraFlags);
	};
	
	SetFunctionBaseMemberAccessSpecifier(InAccessSpecifier, InFunction, InFunctionEntryNode, InBlueprint, FunctionToSetEntryNodeAccessSpecifier);
}

#if EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
void FBlueprintMemberUtility::SetCustomEventAccessSpecifier(const ETsubasamusuAccessSpecifier InAccessSpecifier, UFunction* InFunction, UK2Node_CustomEvent* InCustomEventEntryNode, UBlueprint* InBlueprint)
{
	auto FunctionToSetEntryNodeAccessSpecifier = [InCustomEventEntryNode](const EFunctionFlags InAccessSpecifierFlag, const EFunctionFlags InClearAccessSpecifierMask)
	{
		InCustomEventEntryNode->FunctionFlags &= InClearAccessSpecifierMask;
		InCustomEventEntryNode->FunctionFlags |= InAccessSpecifierFlag;
	};
	
	SetFunctionBaseMemberAccessSpecifier(InAccessSpecifier, InFunction, InCustomEventEntryNode, InBlueprint, FunctionToSetEntryNodeAccessSpecifier);
}
#endif

#undef LOCTEXT_NAMESPACE
