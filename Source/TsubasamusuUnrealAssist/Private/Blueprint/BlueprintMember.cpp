// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "BlueprintMember.h"
#include "BlueprintMemberUtility.h"
#include "K2Node_CallFunction.h"
#include "K2Node_ComponentBoundEvent.h"
#include "K2Node_CreateDelegate.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_GetClassDefaults.h"
#include "K2Node_Variable.h"
#include "Algo/AnyOf.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Type/TsubasamusuUnrealAssistStructs.h"
#include "Type/TsubasamusuUnrealAssistMacros.h"

#if EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
#include "K2Node_CustomEvent.h"
#endif

#define LOCTEXT_NAMESPACE "FBlueprintMember"

void FBlueprintMember::AddReferencedObjects(FReferenceCollector& InReferenceCollector)
{
	InReferenceCollector.AddReferencedObject(OwnerBlueprint);
	InReferenceCollector.AddReferencedObjects(MemberReferencerBlueprints);
	InReferenceCollector.AddReferencedObjects(ReferencerBlueprints);
}

void FBlueprintMember::Initialize()
{
	// Check Member Referencer Blueprints
	{
		MemberReferencerBlueprints = ReferencerBlueprints;
		MemberReferencerBlueprints.RemoveAll([this](const TObjectPtr<const UBlueprint> InMemberReferencerBlueprint)
		{
			return !IsMemberReferencerBlueprint(InMemberReferencerBlueprint);
		});
		
		bCheckedMemberReferencerBlueprints = true;
	}
	
	AccessSpecifierOptimizationRow = MakeShared<FAccessSpecifierOptimizationRow>(GetMemberName(), GetCurrentAccessSpecifier(), GetOptimalAccessSpecifier());
}

ETsubasamusuAccessSpecifier FBlueprintMember::GetOptimalAccessSpecifier() const
{
	if (!CheckedMemberReferencerBlueprints())
	{
		return ETsubasamusuAccessSpecifier::None;
	}
	
	if (MemberReferencerBlueprints.IsEmpty())
	{
		return ETsubasamusuAccessSpecifier::Private;
	}
	
	for (const TObjectPtr<const UBlueprint> MemberReferencerBlueprint : MemberReferencerBlueprints)
	{
		const UClass* MemberReferencerClass = MemberReferencerBlueprint->SkeletonGeneratedClass;
		const UClass* OwnerClass = OwnerBlueprint->SkeletonGeneratedClass;
	
		if (!MemberReferencerClass->IsChildOf(OwnerClass))
		{
			return ETsubasamusuAccessSpecifier::Public;
		}
	}
	
	return ETsubasamusuAccessSpecifier::Protected;
}

ETsubasamusuAccessSpecifier FBlueprintMember_Variable::GetCurrentAccessSpecifier() const
{
	if (!Variable)
	{
		return ETsubasamusuAccessSpecifier::None;
	}
	
	if (Variable->GetBoolMetaData(FBlueprintMetadata::MD_Private))
	{
		return ETsubasamusuAccessSpecifier::Private;
	}
	
	if (Variable->GetBoolMetaData(FBlueprintMetadata::MD_Protected))
	{
		return ETsubasamusuAccessSpecifier::Protected;
	}

	return ETsubasamusuAccessSpecifier::Public;
}

ETsubasamusuAccessSpecifier FBlueprintMember_Variable::GetOptimalAccessSpecifier() const
{
	if (!CheckedMemberReferencerBlueprints())
	{
		return ETsubasamusuAccessSpecifier::None;
	}
	
	return MemberReferencerBlueprints.IsEmpty() ? ETsubasamusuAccessSpecifier::Private : ETsubasamusuAccessSpecifier::Public;
}

void FBlueprintMember_Variable::SetAccessSpecifier(const ETsubasamusuAccessSpecifier InAccessSpecifier)
{
	FBlueprintMemberUtility::SetVariableAccessSpecifier(InAccessSpecifier, GetMemberName(), OwnerBlueprint);
}

FName FBlueprintMember_Variable::GetMemberName() const
{
	if (Variable)
	{
		return Variable->GetFName();
	}
	
	return NAME_None;
}

bool FBlueprintMember_Variable::IsMemberReferencerBlueprint(const UBlueprint* InBlueprint) const
{
	if (!Variable || !IsValid(OwnerBlueprint) || !IsValid(InBlueprint))
	{
		return false;
	}
	
	const FName VariableNameToCheck = GetMemberName();
	
	FGuid VariableGuidToCheck;
	UBlueprint::GetGuidFromClassByFieldName<FProperty>(OwnerBlueprint->SkeletonGeneratedClass, VariableNameToCheck, VariableGuidToCheck);
	check(VariableGuidToCheck.IsValid());
	
	TArray<UEdGraph*> AllGraphsToCheck;
	InBlueprint->GetAllGraphs(AllGraphsToCheck);

	for (TArray<UEdGraph*>::TConstIterator GraphConstIterator(AllGraphsToCheck); GraphConstIterator; ++GraphConstIterator)
	{
		const UEdGraph* GraphToCheck = *GraphConstIterator;

		if (!IsValid(GraphToCheck))
		{
			continue;
		}
		
		// Check variable nodes
		{
			TArray<const UK2Node_Variable*> VariableNodesInGraph;
			GraphToCheck->GetNodesOfClass(VariableNodesInGraph);
		
			auto IsVariableNodeReferencesVariable = [&VariableGuidToCheck, &VariableNameToCheck](const UK2Node_Variable* InVariableNode)
			{
				return VariableGuidToCheck == InVariableNode->VariableReference.GetMemberGuid() && VariableNameToCheck == InVariableNode->GetVarName();
			};

			if (Algo::AnyOf(VariableNodesInGraph, IsVariableNodeReferencesVariable))
			{
				return true;
			}
		}

		// Check class default nodes
		{
			TArray<const UK2Node_GetClassDefaults*> ClassDefaultsNodesInGraph;
			GraphToCheck->GetNodesOfClass(ClassDefaultsNodesInGraph);
		
			auto IsClassDefaultsNodeReferencesVariable = [&VariableNameToCheck, this](const UK2Node_GetClassDefaults* InClassDefaultsNode)
			{
				if (InClassDefaultsNode->GetInputClass() == OwnerBlueprint->SkeletonGeneratedClass)
				{
					const UEdGraphPin* VariablePin = InClassDefaultsNode->FindPin(VariableNameToCheck);
				
					if (VariablePin && VariablePin->Direction == EGPD_Output && VariablePin->LinkedTo.Num() > 0)
					{
						return true;
					}
				}

				return false;
			};
		
			if (Algo::AnyOf(ClassDefaultsNodesInGraph, IsClassDefaultsNodeReferencesVariable))
			{
				return true;
			}
		}

		// Check component bound event nodes
		{
			TArray<const UK2Node_ComponentBoundEvent*> ComponentBoundEventNodes;
			GraphToCheck->GetNodesOfClass(ComponentBoundEventNodes);
		
			auto IsComponentBoundEventNodeReferencesVariable = [&VariableNameToCheck](const UK2Node_ComponentBoundEvent* InComponentBoundEventNode)
			{
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 4, 0)
				return InComponentBoundEventNode->GetComponentPropertyName() == VariableNameToCheck;
#else
				return InComponentBoundEventNode->ComponentPropertyName == VariableNameToCheck;
#endif
			};
		
			if (Algo::AnyOf(ComponentBoundEventNodes, IsComponentBoundEventNodeReferencesVariable))
			{
				return true;
			}
		}

		// Check other K2 nodes
		{
			TArray<const UK2Node*> NodesInGraph;
			GraphToCheck->GetNodesOfClass(NodesInGraph);
		
			auto IsNodeReferencesVariable = [&VariableNameToCheck, this](const UK2Node* InNode)
			{
				return InNode->ReferencesVariable(VariableNameToCheck, OwnerBlueprint->SkeletonGeneratedClass);
			};
		
			if (Algo::AnyOf(NodesInGraph, IsNodeReferencesVariable))
			{
				return true;
			}
		}
	}

	return false;
}

void FBlueprintMember_FunctionBase::AddReferencedObjects(FReferenceCollector& InReferenceCollector)
{
	FBlueprintMember::AddReferencedObjects(InReferenceCollector);
	InReferenceCollector.AddReferencedObject(EntryNode);
	InReferenceCollector.AddReferencedObject(Function);
}

ETsubasamusuAccessSpecifier FBlueprintMember_FunctionBase::GetCurrentAccessSpecifier() const
{
	if (!IsValid(Function))
	{
		return ETsubasamusuAccessSpecifier::None;
	}
	
	if (Function->HasAnyFunctionFlags(FUNC_Private))
	{
		return ETsubasamusuAccessSpecifier::Private;
	}
	
	if (Function->HasAnyFunctionFlags(FUNC_Protected))
	{
		return ETsubasamusuAccessSpecifier::Protected;
	}
	
	return ETsubasamusuAccessSpecifier::Public;
}

void FBlueprintMember_FunctionBase::SetAccessSpecifier(const ETsubasamusuAccessSpecifier InAccessSpecifier)
{
	if(!IsValid(Function) || !IsValid(EntryNode) || !IsValid(OwnerBlueprint))
	{
		return;
	}
	
	const FScopedTransaction Transaction(LOCTEXT("ChangeFunctionAccessSpecifierTransaction", "Change Function Access Specifier"));

	EntryNode->Modify();
	Function->Modify();

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
	SetEntryNodeAccessSpecifier(AccessSpecifierFlag, ClearAccessSpecifierMask);
	
	Function->FunctionFlags &= ClearAccessSpecifierMask;
	Function->FunctionFlags |= AccessSpecifierFlag;

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(OwnerBlueprint);
}

FName FBlueprintMember_FunctionBase::GetMemberName() const
{
	if (IsValid(Function))
	{
		return Function->GetFName();
	}
	
	return NAME_None;
}

bool FBlueprintMember_FunctionBase::IsMemberReferencerBlueprint(const UBlueprint* InBlueprint) const
{
	if (!IsValid(Function) || !IsValid(OwnerBlueprint) || !IsValid(InBlueprint))
	{
		return false;
	}
	
	const FName FunctionNameToCheck = GetMemberName();
	
	// Check whether that function is overridden
	{
		const UClass* ClassToCheck = InBlueprint->SkeletonGeneratedClass;
		const UClass* OwnerClass = OwnerBlueprint->SkeletonGeneratedClass;
		
		if (ClassToCheck->IsChildOf(OwnerClass))
		{
			for (const UClass* SuperClass = ClassToCheck; IsValid(SuperClass) && SuperClass != OwnerClass; SuperClass = SuperClass->GetSuperClass())
			{
				const UFunction* OverriddenFunction = SuperClass->FindFunctionByName(FunctionNameToCheck, EIncludeSuperFlag::ExcludeSuper);
				
				if (IsValid(OverriddenFunction))
				{
					return true;
				}
			}
		}
	}
	
	FGuid FunctionGuidToCheck;
	UBlueprint::GetFunctionGuidFromClassByFieldName(OwnerBlueprint->SkeletonGeneratedClass, FunctionNameToCheck, FunctionGuidToCheck);
	check(FunctionGuidToCheck.IsValid());
	
	TArray<UEdGraph*> AllGraphsToCheck;
	InBlueprint->GetAllGraphs(AllGraphsToCheck);

	for (TArray<UEdGraph*>::TConstIterator GraphConstIterator(AllGraphsToCheck); GraphConstIterator; ++GraphConstIterator)
	{
		const UEdGraph* GraphToCheck = *GraphConstIterator;
		
		if (!IsValid(GraphToCheck))
		{
			continue;
		}
		
		// Check call function nodes
		{
			TArray<const UK2Node_CallFunction*> CallFunctionNodesInGraph;
			GraphToCheck->GetNodesOfClass(CallFunctionNodesInGraph);
			
			auto IsCallFunctionNodeReferencesFunction = [&FunctionGuidToCheck, &FunctionNameToCheck](const UK2Node_CallFunction* InCallFunctionNode)
			{
				return FunctionGuidToCheck == InCallFunctionNode->FunctionReference.GetMemberGuid() && FunctionNameToCheck == InCallFunctionNode->GetFunctionName();
			};

			if (Algo::AnyOf(CallFunctionNodesInGraph, IsCallFunctionNodeReferencesFunction))
			{
				return true;
			}
		}
		
		// Check create delegate nodes
		{
			TArray<const UK2Node_CreateDelegate*> CreateDelegateNodesInGraph;
			GraphToCheck->GetNodesOfClass(CreateDelegateNodesInGraph);
			
			auto IsCreateDelegateNodeReferencesFunction = [&FunctionGuidToCheck, &FunctionNameToCheck](const UK2Node_CreateDelegate* InCreateDelegateNode)
			{
				return FunctionGuidToCheck == InCreateDelegateNode->SelectedFunctionGuid && FunctionNameToCheck == InCreateDelegateNode->GetFunctionName();
			};

			if (Algo::AnyOf(CreateDelegateNodesInGraph, IsCreateDelegateNodeReferencesFunction))
			{
				return true;
			}
		}

		// Check other K2 nodes
		{
			TArray<const UK2Node*> NodesInGraph;
			GraphToCheck->GetNodesOfClass(NodesInGraph);
			
			auto IsNodeReferencesFunction = [&FunctionNameToCheck, this](const UK2Node* InNode)
			{
				return InNode->ReferencesFunction(FunctionNameToCheck, OwnerBlueprint->SkeletonGeneratedClass);
			};

			if (Algo::AnyOf(NodesInGraph, IsNodeReferencesFunction))
			{
				return true;
			}
		}
	}

	return false;
}

void FBlueprintMember_Function::SetEntryNodeAccessSpecifier(const EFunctionFlags InAccessSpecifierFlag, const EFunctionFlags InClearAccessSpecifierMask)
{
	UK2Node_FunctionEntry* FunctionEntryNode = GetEntryNodeChecked<UK2Node_FunctionEntry>();
	int32 ExtraFlags = FunctionEntryNode->GetExtraFlags();
	ExtraFlags &= InClearAccessSpecifierMask;
	ExtraFlags |= InAccessSpecifierFlag;
	FunctionEntryNode->SetExtraFlags(ExtraFlags);
}

#if EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
void FBlueprintMember_Event::SetEntryNodeAccessSpecifier(const EFunctionFlags InAccessSpecifierFlag, const EFunctionFlags InClearAccessSpecifierMask)
{
	UK2Node_CustomEvent* EventEntryNode = GetEntryNodeChecked<UK2Node_CustomEvent>();
	EventEntryNode->FunctionFlags &= InClearAccessSpecifierMask;
	EventEntryNode->FunctionFlags |= InAccessSpecifierFlag;
}
#endif

#undef LOCTEXT_NAMESPACE
