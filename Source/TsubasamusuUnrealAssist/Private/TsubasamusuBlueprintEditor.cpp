// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "TsubasamusuBlueprintEditor.h"
#include "K2Node_ComponentBoundEvent.h"
#include "K2Node_GetClassDefaults.h"
#include "K2Node_Variable.h"
#include "TsubasamusuBlueprintEditorCommands.h"
#include "Algo/AnyOf.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/TimelineComponent.h"
#include "Engine/LevelScriptBlueprint.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "TsubasamusuUnrealAssist"

void FTsubasamusuBlueprintEditor::InitTsubasamusuBlueprintEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, const TArray<UBlueprint*>& InBlueprints, bool bShouldOpenInDefaultsMode)
{
	InitBlueprintEditor(Mode, InitToolkitHost, InBlueprints, bShouldOpenInDefaultsMode);
	RegisterOriginalMenus();
}

TWeakPtr<SGraphEditor> FTsubasamusuBlueprintEditor::GetFocusedGraphEditor() const
{
	return FocusedGraphEdPtr;
}

void FTsubasamusuBlueprintEditor::CreateDefaultCommands()
{
	FBlueprintEditor::CreateDefaultCommands();
	
	FTsubasamusuBlueprintEditorCommands::Register();
	
	ToolkitCommands->MapAction(FTsubasamusuBlueprintEditorCommands::Get().ChangeMemberVariablesAccessSpecifierToPrivate,
		FExecuteAction::CreateSP(this, &FTsubasamusuBlueprintEditor::ChangeMemberVariablesAccessSpecifierToPrivate_OnClicked),
		FCanExecuteAction::CreateSP(this, &FBlueprintEditor::IsInAScriptingMode));
}

void FTsubasamusuBlueprintEditor::RegisterOriginalMenus() const
{
	const FName EditMenuName = *(GetToolMenuName().ToString() + TEXT(".Edit"));
	const FName ParentEditMenuName = TEXT("MainFrame.MainMenu.Edit");
	
	UToolMenu* ToolMenu = UToolMenus::Get()->RegisterMenu(EditMenuName, ParentEditMenuName, EMultiBoxType::Menu, false);
	
	const FName SectionName = TEXT("EditTsubasamusuUnrealAssist");
	const FText SectionLabel = LOCTEXT("EditMenu_TsubasamusuUnrealAssist", "Tsubasamusu Unreal Assist");
	const FName AboveSectionName = TEXT("EditSearch");
	
	FToolMenuSection& ToolMenuSection = ToolMenu->AddSection(SectionName, SectionLabel);
	ToolMenuSection.InsertPosition = FToolMenuInsert(AboveSectionName, EToolMenuInsertType::After);
	
	ToolMenuSection.AddMenuEntry(FTsubasamusuBlueprintEditorCommands::Get().ChangeMemberVariablesAccessSpecifierToPrivate);
}

void FTsubasamusuBlueprintEditor::ChangeMemberVariablesAccessSpecifierToPrivate_OnClicked()
{
}

TArray<FProperty*> FTsubasamusuBlueprintEditor::GetMemberVariables(const UBlueprint* InBlueprint)
{
	TArray<FProperty*> MemberVariables;
	
	for (TFieldIterator<FProperty> PropertyFieldIterator(InBlueprint->SkeletonGeneratedClass, EFieldIteratorFlags::ExcludeSuper); PropertyFieldIterator; ++PropertyFieldIterator)
	{
		FProperty* Property = *PropertyFieldIterator;
		
		const bool bIsDelegateProperty = Property->IsA(FDelegateProperty::StaticClass()) || Property->IsA(FMulticastDelegateProperty::StaticClass());
		const bool bIsFunctionParameter = Property->HasAnyPropertyFlags(CPF_Parm);
		const bool bIsBlueprintVisibleProperty = Property->HasAllPropertyFlags(CPF_BlueprintVisible);
		const bool bIsMemberVariable = !bIsFunctionParameter && bIsBlueprintVisibleProperty && !bIsDelegateProperty;

		if (bIsMemberVariable)
		{
			const FName VariableName = Property->GetFName();
			const int32 VariableIndex = FBlueprintEditorUtils::FindNewVariableIndex(InBlueprint, VariableName);
			const bool bFoundVariable = VariableIndex != INDEX_NONE;

			const FObjectPropertyBase* ObjectPropertyBase = CastField<const FObjectPropertyBase>(Property);
			const bool bIsTimelineComponent = ObjectPropertyBase && ObjectPropertyBase->PropertyClass && ObjectPropertyBase->PropertyClass->IsChildOf(UTimelineComponent::StaticClass());
	
			if (bFoundVariable && !bIsTimelineComponent)
			{
				MemberVariables.Add(Property);
			}
		}
	}
	
	return MemberVariables;
}

bool FTsubasamusuBlueprintEditor::IsVariableReferencedFromAnyOtherClass(const FProperty* InVariable, const UBlueprint* VariableOwnerBlueprint)
{
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	
	FARFilter AssetRegistryFilter;
	AssetRegistryModule.Get().GetReferencers(VariableOwnerBlueprint->GetPackage()->GetFName(), AssetRegistryFilter.PackageNames, UE::AssetRegistry::EDependencyCategory::Package, UE::AssetRegistry::EDependencyQuery::Hard);

	if (AssetRegistryFilter.PackageNames.Num() == 0)
	{
		return false;
	}
	
	GWarn->BeginSlowTask(LOCTEXT("LoadingReferencerAssets", "Loading Referencing Assets ..."), true);
	{
		AssetRegistryFilter.TagsAndValues.Add(FBlueprintTags::IsDataOnly, TOptional<FString>(TEXT("false")));
		TArray<FAssetData> ReferencerAssetDataArray;
		AssetRegistryModule.Get().GetAssets(AssetRegistryFilter, ReferencerAssetDataArray);

		for (const FAssetData& ReferencerAssetData : ReferencerAssetDataArray)
		{
			const UObject* ReferencerAsset = ReferencerAssetData.GetAsset();
			const UBlueprint* ReferencerBlueprint = Cast<const UBlueprint>(ReferencerAsset);

			if (IsValid(ReferencerBlueprint) && ReferencerBlueprint != VariableOwnerBlueprint && IsBlueprintReferencesVariable(ReferencerBlueprint, InVariable, VariableOwnerBlueprint))
			{
				GWarn->EndSlowTask();
				return true;
			}
			
			const UWorld* ReferencerWorld = Cast<const UWorld>(ReferencerAsset);
			
			if (IsValid(ReferencerWorld))
			{
				const TObjectPtr<ULevel>& PersistentLevel = ReferencerWorld->PersistentLevel;
				
				if (IsValid(PersistentLevel) && IsValid(PersistentLevel->OwningWorld))
				{
					const ULevelScriptBlueprint* PersistentLevelBlueprint = PersistentLevel->GetLevelScriptBlueprint();
				
					if (PersistentLevelBlueprint != VariableOwnerBlueprint && IsBlueprintReferencesVariable(PersistentLevelBlueprint, InVariable, VariableOwnerBlueprint))
					{
						GWarn->EndSlowTask();
						return true;
					}
				}
			}
		}
	}
	
	GWarn->EndSlowTask();
	return false;
}

bool FTsubasamusuBlueprintEditor::IsBlueprintReferencesVariable(const UBlueprint* BlueprintToCheck, const FProperty* VariableToCheck, const UBlueprint* VariableOwnerBlueprint)
{
	const FName VariableNameToCheck = VariableToCheck->GetFName();
	
	FGuid VariableGuidToCheck;
	UBlueprint::GetGuidFromClassByFieldName<FProperty>(VariableOwnerBlueprint->SkeletonGeneratedClass, VariableNameToCheck, VariableGuidToCheck);
	check(VariableGuidToCheck.IsValid());
	
	TArray<UEdGraph*> AllGraphsToCheck;
	BlueprintToCheck->GetAllGraphs(AllGraphsToCheck);

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
		
			auto IsClassDefaultsNodeReferencesVariable = [&VariableNameToCheck, &VariableOwnerBlueprint](const UK2Node_GetClassDefaults* InClassDefaultsNode)
			{
				if (InClassDefaultsNode->GetInputClass() == VariableOwnerBlueprint->SkeletonGeneratedClass)
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
				return InComponentBoundEventNode->GetComponentPropertyName() == VariableNameToCheck;
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
		
			auto IsNodeReferencesVariable = [&VariableNameToCheck, &VariableOwnerBlueprint](const UK2Node* InNode)
			{
				return InNode->ReferencesVariable(VariableNameToCheck, VariableOwnerBlueprint->SkeletonGeneratedClass);
			};
		
			if (Algo::AnyOf(NodesInGraph, IsNodeReferencesVariable))
			{
				return true;
			}
		}
	}

	return false;
}

#undef LOCTEXT_NAMESPACE
