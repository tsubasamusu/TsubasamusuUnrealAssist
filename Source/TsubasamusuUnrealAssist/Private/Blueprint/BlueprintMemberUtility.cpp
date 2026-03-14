// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "BlueprintMemberUtility.h"
#include "K2Node_FunctionEntry.h"
#include "Components/TimelineComponent.h"

#if EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
#include "K2Node_CustomEvent.h"
#endif

#define LOCTEXT_NAMESPACE "FBlueprintMemberUtility"

template<typename FunctionToFindGraph, typename FunctionToCheckEditablePinNode, typename EntryNodeType>
static TMap<UFunction*, EntryNodeType*> GetFunctionBaseMembers(UBlueprint* InBlueprint, const FunctionToFindGraph& InFunctionToFindGraph, const FunctionToCheckEditablePinNode& InFunctionToCheckEditablePinNode)
{
	static_assert(TIsDerivedFrom<EntryNodeType, UK2Node_EditablePinBase>::Value, "EntryNodeType must be derived from UK2Node_EditablePinBase.");
		
	TMap<UFunction*, EntryNodeType*> FunctionBaseMembers;
	
	auto FunctionForEachFunctionBaseMembers = [&FunctionBaseMembers](UFunction* InFunction, UEdGraph* /*InGraph*/, UK2Node_EditablePinBase* InEditablePinNode)
	{
		EntryNodeType* EntryNode = Cast<EntryNodeType>(InEditablePinNode);
			
		if (IsValid(EntryNode))
		{
			FunctionBaseMembers.Add(InFunction, EntryNode);
		}
	};
		
	FBlueprintMemberUtility::ForEachFunctionBaseMembers(InBlueprint, InFunctionToFindGraph, FunctionForEachFunctionBaseMembers, InFunctionToCheckEditablePinNode);
	return FunctionBaseMembers;
}

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

TArray<FProperty*> FBlueprintMemberUtility::GetVariables(const UBlueprint* InBlueprint)
{
	TArray<FProperty*> Variables;
	
	for (TFieldIterator<FProperty> PropertyFieldIterator(InBlueprint->SkeletonGeneratedClass, EFieldIterationFlags::None); PropertyFieldIterator; ++PropertyFieldIterator)
	{
		FProperty* Property = *PropertyFieldIterator;
		
		if (Property)
		{
			const bool bIsDelegateProperty = Property->IsA(FDelegateProperty::StaticClass()) || Property->IsA(FMulticastDelegateProperty::StaticClass());
			const bool bIsFunctionParameter = Property->HasAnyPropertyFlags(CPF_Parm);
			const bool bIsBlueprintVisibleProperty = Property->HasAllPropertyFlags(CPF_BlueprintVisible);
		
			if (!bIsFunctionParameter && bIsBlueprintVisibleProperty && !bIsDelegateProperty)
			{
				const int32 VariableIndex = FBlueprintEditorUtils::FindNewVariableIndex(InBlueprint, Property->GetFName());
				const bool bFoundVariable = VariableIndex != INDEX_NONE;

				const FObjectPropertyBase* ObjectPropertyBase = CastField<const FObjectPropertyBase>(Property);
				const bool bIsTimelineComponent = ObjectPropertyBase && ObjectPropertyBase->PropertyClass && ObjectPropertyBase->PropertyClass->IsChildOf(UTimelineComponent::StaticClass());
	
				if (bFoundVariable && !bIsTimelineComponent)
				{
					Variables.Add(Property);
				}
			}
		}
	}
	
	return Variables;
}

TMap<UFunction*, UK2Node_FunctionEntry*> FBlueprintMemberUtility::GetFunctions(UBlueprint* InBlueprint)
{
	if (IsValid(InBlueprint))
	{
		auto FunctionToFindGraph = [](const FName& InFunctionName, UBlueprint* InBlueprintToFindGraph) -> UEdGraph*
		{
			return FindFunctionGraph(InFunctionName, InBlueprintToFindGraph);
		};
		
		auto FunctionToCheckEditablePinNode = [](const FName& /*InFunctionOrEventName*/, const UK2Node_EditablePinBase* InEditablePinNode)
		{
			return IsFunctionEntryNode(InEditablePinNode);
		};
	
		return GetFunctionBaseMembers<decltype(FunctionToFindGraph), decltype(FunctionToCheckEditablePinNode), UK2Node_FunctionEntry>(InBlueprint, FunctionToFindGraph, FunctionToCheckEditablePinNode);
	}
	
	return TMap<UFunction*, UK2Node_FunctionEntry*>();
}

#if EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
TMap<UFunction*, UK2Node_CustomEvent*> FBlueprintMemberUtility::GetEvents(UBlueprint* InBlueprint)
{
	if (IsValid(InBlueprint))
	{
		auto FunctionToCheckEditablePinNode = [](const FName& InFunctionOrEventName, const UK2Node_EditablePinBase* InEditablePinNode)
		{
			return
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 3, 0)
				InEditablePinNode->GetNodeTitle(ENodeTitleType::Type::ListView).ToString() == InFunctionOrEventName;
#else 
				InEditablePinNode->GetNodeTitle(ENodeTitleType::Type::ListView).ToString() == InFunctionOrEventName.ToString();
#endif
		};
		
		auto FunctionToFindGraph = [&FunctionToCheckEditablePinNode](const FName& InFunctionName, UBlueprint* InBlueprintToFindGraph) -> UEdGraph*
		{
			for (const TObjectPtr<UEdGraph> EventGraph : InBlueprintToFindGraph->UbergraphPages)
			{
				if (IsValid(EventGraph))
				{
					TArray<UK2Node_CustomEvent*> CustomEventNodes;
					EventGraph->GetNodesOfClass(CustomEventNodes);
				
					for (const UK2Node_CustomEvent* CustomEventNode : CustomEventNodes)
					{
						if (FunctionToCheckEditablePinNode(InFunctionName, CustomEventNode))
						{
							return EventGraph;
						}
					}
				}
			}

			return nullptr;
		};
	
		return GetFunctionBaseMembers<decltype(FunctionToFindGraph), decltype(FunctionToCheckEditablePinNode), UK2Node_CustomEvent>(InBlueprint, FunctionToFindGraph, FunctionToCheckEditablePinNode);
	}
	
	return TMap<UFunction*, UK2Node_CustomEvent*>();
}
#endif

#undef LOCTEXT_NAMESPACE
