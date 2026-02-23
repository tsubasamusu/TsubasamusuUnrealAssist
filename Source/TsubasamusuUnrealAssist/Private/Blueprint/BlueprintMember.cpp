// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "BlueprintMember.h"
#include "K2Node_ComponentBoundEvent.h"
#include "K2Node_GetClassDefaults.h"
#include "K2Node_Variable.h"
#include "Algo/AnyOf.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Type/TsubasamusuUnrealAssistStructs.h"

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

TsubasamusuUnrealAssist::EAccessSpecifier FBlueprintMember::GetOptimalAccessSpecifier() const
{
	if (!CheckedMemberReferencerBlueprints())
	{
		return TsubasamusuUnrealAssist::EAccessSpecifier::None;
	}
	
	if (MemberReferencerBlueprints.IsEmpty())
	{
		return TsubasamusuUnrealAssist::EAccessSpecifier::Private;
	}
	
	for (const TObjectPtr<const UBlueprint> MemberReferencerBlueprint : MemberReferencerBlueprints)
	{
		const UClass* MemberReferencerClass = MemberReferencerBlueprint->GeneratedClass;
		const UClass* OwnerClass = OwnerBlueprint->GeneratedClass;
	
		if (!MemberReferencerClass->IsChildOf(OwnerClass))
		{
			return TsubasamusuUnrealAssist::EAccessSpecifier::Public;
		}
	}
	
	return TsubasamusuUnrealAssist::EAccessSpecifier::Protected;
}

TsubasamusuUnrealAssist::EAccessSpecifier FBlueprintMember_Variable::GetCurrentAccessSpecifier() const
{
	if (IsValid(OwnerBlueprint) && Variable)
	{
		FString PrivateMetaDataValue;
		FBlueprintEditorUtils::GetBlueprintVariableMetaData(OwnerBlueprint, GetMemberName(), nullptr, FBlueprintMetadata::MD_Private, PrivateMetaDataValue);
	
		return PrivateMetaDataValue == TEXT("true") ? TsubasamusuUnrealAssist::EAccessSpecifier::Private : TsubasamusuUnrealAssist::EAccessSpecifier::Public;
	}
	
	return TsubasamusuUnrealAssist::EAccessSpecifier::None;
}

TsubasamusuUnrealAssist::EAccessSpecifier FBlueprintMember_Variable::GetOptimalAccessSpecifier() const
{
	if (!CheckedMemberReferencerBlueprints())
	{
		return TsubasamusuUnrealAssist::EAccessSpecifier::None;
	}
	
	return MemberReferencerBlueprints.IsEmpty() ? TsubasamusuUnrealAssist::EAccessSpecifier::Private : TsubasamusuUnrealAssist::EAccessSpecifier::Public;
}

void FBlueprintMember_Variable::SetAccessSpecifier(const TsubasamusuUnrealAssist::EAccessSpecifier InAccessSpecifier)
{
	switch (InAccessSpecifier)
	{
	case TsubasamusuUnrealAssist::EAccessSpecifier::Private:
		FBlueprintEditorUtils::SetBlueprintVariableMetaData(OwnerBlueprint, GetMemberName(), nullptr, FBlueprintMetadata::MD_Private, TEXT("true"));
		break;
	case TsubasamusuUnrealAssist::EAccessSpecifier::Public:
		FBlueprintEditorUtils::RemoveBlueprintVariableMetaData(OwnerBlueprint, GetMemberName(), nullptr, FBlueprintMetadata::MD_Private);
		break;
	default:
		checkNoEntry()
		break;
	}
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
	
	const FName VariableNameToCheck = Variable->GetFName();
	
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
			TArray<UK2Node_Variable*> VariableNodesInGraph;
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
			TArray<UK2Node_GetClassDefaults*> ClassDefaultsNodesInGraph;
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
			TArray<UK2Node_ComponentBoundEvent*> ComponentBoundEventNodes;
			GraphToCheck->GetNodesOfClass(ComponentBoundEventNodes);
		
			auto IsComponentBoundEventNodeReferencesVariable = [&VariableNameToCheck](const UK2Node_ComponentBoundEvent* InComponentBoundEventNode)
			{
#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 4)
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
