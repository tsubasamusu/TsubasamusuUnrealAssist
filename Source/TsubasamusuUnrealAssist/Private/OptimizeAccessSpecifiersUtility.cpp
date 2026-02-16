// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "OptimizeAccessSpecifiersUtility.h"
#include "K2Node_ComponentBoundEvent.h"
#include "K2Node_GetClassDefaults.h"
#include "K2Node_Variable.h"
#include "SAccessSpecifierOptimizationRow.h"
#include "TsubasamusuBlueprintEditorCommands.h"
#include "TsubasamusuLogUtility.h"
#include "Algo/AnyOf.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/TimelineComponent.h"
#include "Engine/LevelScriptBlueprint.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Toolkits/ToolkitManager.h"

#define LOCTEXT_NAMESPACE "TsubasamusuUnrealAssist"

void FOptimizeAccessSpecifiersUtility::OnBlueprintEditorOpened(UBlueprint* InOpenedBlueprint)
{
	const TSharedPtr<IToolkit> Toolkit = FToolkitManager::Get().FindEditorForAsset(InOpenedBlueprint);
	
	if (Toolkit.IsValid())
	{
		FTsubasamusuBlueprintEditorCommands::Register();
		
		const TSharedPtr<FBlueprintEditor> BlueprintEditor = StaticCastSharedPtr<FBlueprintEditor>(Toolkit);
		const TSharedPtr<FUICommandList> ToolkitCommands = BlueprintEditor->GetToolkitCommands();
		
		ToolkitCommands->MapAction(FTsubasamusuBlueprintEditorCommands::Get().OptimizeAccessSpecifiers,
			FExecuteAction::CreateStatic(&OnOptimizeAccessSpecifiersClicked, BlueprintEditor),
			FCanExecuteAction::CreateSP(BlueprintEditor.Get(), &FBlueprintEditor::IsInAScriptingMode));
		
		RegisterAdditionalMenus(BlueprintEditor);
	}
}

void FOptimizeAccessSpecifiersUtility::RegisterAdditionalMenus(const TSharedPtr<FBlueprintEditor> InBlueprintEditor)
{
	const FName EditMenuName = *(InBlueprintEditor->GetToolMenuName().ToString() + TEXT(".Edit"));
	const FName ParentEditMenuName = TEXT("MainFrame.MainMenu.Edit");
	
	UToolMenu* ToolMenu = UToolMenus::Get()->RegisterMenu(EditMenuName, ParentEditMenuName, EMultiBoxType::Menu, false);
	
	const FName SectionName = TEXT("EditTsubasamusuUnrealAssist");
	const FText SectionLabel = LOCTEXT("EditMenu_TsubasamusuUnrealAssist", "Tsubasamusu Unreal Assist");
	const FName AboveSectionName = TEXT("EditSearch");
	
	FToolMenuSection& ToolMenuSection = ToolMenu->AddSection(SectionName, SectionLabel);
	ToolMenuSection.InsertPosition = FToolMenuInsert(AboveSectionName, EToolMenuInsertType::After);
	
	ToolMenuSection.AddMenuEntry(FTsubasamusuBlueprintEditorCommands::Get().OptimizeAccessSpecifiers);
}

void FOptimizeAccessSpecifiersUtility::OnOptimizeAccessSpecifiersClicked(const TSharedPtr<FBlueprintEditor> InBlueprintEditor)
{
	UBlueprint* CurrentlyOpenBlueprint = InBlueprintEditor->GetBlueprintObj();
	check(IsValid(CurrentlyOpenBlueprint));
	
	TArray<FProperty*> Variables = GetVariables(CurrentlyOpenBlueprint);
	
	if (Variables.IsEmpty())
	{
		const FText NotificationText = LOCTEXT("NoVariablesToCheck", "No variables to check for.");
		FTsubasamusuLogUtility::DisplaySimpleNotification(NotificationText);
		return;
	}
	
	RemoveVariablesShouldNotBePrivate(Variables, CurrentlyOpenBlueprint);
	
	if (Variables.IsEmpty())
	{
		const FText NotificationText = LOCTEXT("NoVariablesShouldBePrivate", "There are no variables that should be private.");
		FTsubasamusuLogUtility::DisplaySimpleNotification(NotificationText);
		return;
	}
	
	TSharedPtr<TArray<TSharedPtr<FAccessSpecifierOptimizationRow>>> RowItems = MakeShared<TArray<TSharedPtr<FAccessSpecifierOptimizationRow>>>();
	
	for (const FProperty* Variable : Variables)
	{
		const TSharedPtr<FAccessSpecifierOptimizationRow> RowItem = MakeShared<FAccessSpecifierOptimizationRow>(
			Variable->GetFName(),
			TsubasamusuBlueprintEditor::EMemberType::Variable,
			GetCurrentAccessSpecifier(Variable, CurrentlyOpenBlueprint),
			GetOptimalAccessSpecifier(Variable, CurrentlyOpenBlueprint));
		
		RowItems->Add(RowItem);
	}

	const FName CheckBoxColumnId = TEXT("CheckBox");
	const FName MemberNameColumnId = TEXT("MemberName");
	const FName CurrentAccessSpecifierColumnId = TEXT("CurrentAccessSpecifier");
	const FName RecommendedAccessSpecifierColumnId = TEXT("RecommendedAccessSpecifier");
	
	const TSharedRef<SListView<TSharedPtr<FAccessSpecifierOptimizationRow>>> DialogContent = SNew(SListView<TSharedPtr<FAccessSpecifierOptimizationRow>>)
		.ListItemsSource(RowItems.Get())
		.SelectionMode(ESelectionMode::Type::None)
		.HeaderRow
		(
			SNew(SHeaderRow)
			+ SHeaderRow::Column(CheckBoxColumnId)
			.DefaultLabel(FText::GetEmpty())
			.FixedWidth(30.f)
			[
				SNew(SBox)
				.HAlign(HAlign_Center)
				[
					SNew(SCheckBox)
					.IsChecked_Lambda([RowItems]()
					{
						bool bFoundSelectedRowItem = false;
						bool bFoundUnselectedRowItem = false;

						if (RowItems.IsValid())
						{
							for (const TSharedPtr<FAccessSpecifierOptimizationRow> RowItem : *RowItems)
							{
								if (!RowItem.IsValid())
								{
									continue;
								}

								if (RowItem->bSelected)
								{
									bFoundSelectedRowItem = true;
								}
								else
								{
									bFoundUnselectedRowItem = true;
								}

								if (bFoundSelectedRowItem && bFoundUnselectedRowItem)
								{
									return ECheckBoxState::Undetermined;
								}
							}
						}

						return bFoundSelectedRowItem ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
					})
					.OnCheckStateChanged_Lambda([RowItems](const ECheckBoxState NewState)
					{
						if (NewState != ECheckBoxState::Undetermined)
						{
							for (const TSharedPtr<FAccessSpecifierOptimizationRow> RowItem : *RowItems)
							{
								if (RowItem->CheckBox.IsValid() && RowItem->CheckBox->GetCheckedState() != NewState)
								{
							  		RowItem->CheckBox->ToggleCheckedState();
								}
							}
						}
					})
				]
			]
			+ SHeaderRow::Column(MemberNameColumnId)
			.DefaultLabel(LOCTEXT("HeaderLabel_MemberName", "Member Name"))
			.HAlignHeader(HAlign_Center)
			+ SHeaderRow::Column(CurrentAccessSpecifierColumnId)
			.DefaultLabel(LOCTEXT("HeaderLabel_CurrentAccessSpecifier", "Current"))
			.HAlignHeader(HAlign_Center)
			.FixedWidth(80.f)
			+ SHeaderRow::Column(RecommendedAccessSpecifierColumnId)
			.DefaultLabel(LOCTEXT("HeaderLabel_RecommendedAccessSpecifier", "Recommended"))
			.HAlignHeader(HAlign_Center)
			.FixedWidth(110.f)
		)
		.OnGenerateRow_Lambda([&CheckBoxColumnId, &MemberNameColumnId, &CurrentAccessSpecifierColumnId, &RecommendedAccessSpecifierColumnId](const TSharedPtr<FAccessSpecifierOptimizationRow> RowItem, const TSharedRef<STableViewBase>& OwnerTableView)
		{
			return SNew(SAccessSpecifierOptimizationRow, OwnerTableView)
				.RowItem(RowItem)
				.CheckBoxColumnId(CheckBoxColumnId)
				.MemberNameColumnId(MemberNameColumnId)
				.CurrentAccessSpecifierColumnId(CurrentAccessSpecifierColumnId)
				.RecommendedAccessSpecifierColumnId(RecommendedAccessSpecifierColumnId);
		});

	const TAttribute<bool> OkButtonIsEnabled = TAttribute<bool>::CreateLambda([RowItems]()
	{
		for (const TSharedPtr<FAccessSpecifierOptimizationRow> RowItem : *RowItems)
		{
			if (RowItem->bSelected)
			{
				return true;
			}
		}
		
		return false;
	});
	
	const FText DialogTitle = LOCTEXT("OptimizeAccessSpecifiersDialog_Title", "Optimize Access Specifiers");
	const FText DialogMessage = LOCTEXT("OptimizeAccessSpecifiersDialog_Message", "You might want to change the access specifiers for these members.");
	const FText ApplyButtonText = LOCTEXT("OptimizeAccessSpecifiersDialog_ApplyButton", "Apply Recommended Access Specifiers");
	const FText CancelButtonText = LOCTEXT("OptimizeAccessSpecifiersDialog_CancelButton", "Cancel");
	
	const FTsubasamusuLogUtility::EDialogButton PressedButton = FTsubasamusuLogUtility::ShowCustomDialog(DialogTitle, DialogMessage, ApplyButtonText, CancelButtonText, DialogContent, OkButtonIsEnabled);

	if (PressedButton != FTsubasamusuLogUtility::EDialogButton::OK)
	{
		return;
	}
	
	for (const TSharedPtr<FAccessSpecifierOptimizationRow> RowItem : *RowItems)
	{
		if (!RowItem->bSelected)
		{
			continue;
		}
		
		if (RowItem->MemberType == TsubasamusuBlueprintEditor::EMemberType::Variable)
		{
			FBlueprintEditorUtils::SetBlueprintVariableMetaData(CurrentlyOpenBlueprint, RowItem->MemberName, nullptr, FBlueprintMetadata::MD_Private, TEXT("true"));
		}
	}
	
	const FText NotificationText = LOCTEXT("SuccessfullyAppliedRecommendedAccessSpecifiers", "Successfully applied recommended access specifiers.");
	FTsubasamusuLogUtility::DisplaySimpleNotification(NotificationText, SNotificationItem::ECompletionState::CS_Success);
}

TArray<FProperty*> FOptimizeAccessSpecifiersUtility::GetVariables(const UBlueprint* InBlueprint)
{
	TArray<FProperty*> Variables;
	
	for (TFieldIterator<FProperty> PropertyFieldIterator(InBlueprint->SkeletonGeneratedClass, EFieldIteratorFlags::ExcludeSuper); PropertyFieldIterator; ++PropertyFieldIterator)
	{
		FProperty* Property = *PropertyFieldIterator;
		
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
	
	return Variables;
}

void FOptimizeAccessSpecifiersUtility::RemoveVariablesShouldNotBePrivate(TArray<FProperty*>& OutVariables, const UBlueprint* VariablesOwnerBlueprint)
{
	OutVariables.RemoveAll([VariablesOwnerBlueprint](const FProperty* InVariable)
	{
		FString PrivateMetaDataValue;
		FBlueprintEditorUtils::GetBlueprintVariableMetaData(VariablesOwnerBlueprint, InVariable->GetFName(), nullptr, FBlueprintMetadata::MD_Private, PrivateMetaDataValue);
		
		return PrivateMetaDataValue == TEXT("true") || !GetBlueprintsReferenceVariable(InVariable, VariablesOwnerBlueprint).IsEmpty();
	});
}

TsubasamusuBlueprintEditor::EAccessSpecifier FOptimizeAccessSpecifiersUtility::GetOptimalAccessSpecifier(const FProperty* InVariable, const UBlueprint* VariableOwnerBlueprint)
{
	const TArray<UBlueprint*> BlueprintsReferencesVariable = GetBlueprintsReferenceVariable(InVariable, VariableOwnerBlueprint);
	
	if (BlueprintsReferencesVariable.IsEmpty())
	{
		return TsubasamusuBlueprintEditor::EAccessSpecifier::Private;
	}
	
	for (const UBlueprint* BlueprintReferencesVariable : BlueprintsReferencesVariable)
	{
		const UClass* BlueprintReferencesVariableClass = BlueprintReferencesVariable->GeneratedClass;
		const UClass* VariableOwnerBlueprintClass = VariableOwnerBlueprint->GeneratedClass;
		
		if (!BlueprintReferencesVariableClass->IsChildOf(VariableOwnerBlueprintClass))
		{
			return TsubasamusuBlueprintEditor::EAccessSpecifier::Public;
		}
	}
	
	return TsubasamusuBlueprintEditor::EAccessSpecifier::Protected;
}

TsubasamusuBlueprintEditor::EAccessSpecifier FOptimizeAccessSpecifiersUtility::GetCurrentAccessSpecifier(const FProperty* InVariable, const UBlueprint* VariableOwnerBlueprint)
{
	FString PrivateMetaDataValue;
	FBlueprintEditorUtils::GetBlueprintVariableMetaData(VariableOwnerBlueprint, InVariable->GetFName(), nullptr, FBlueprintMetadata::MD_Private, PrivateMetaDataValue);
		
	return PrivateMetaDataValue == TEXT("true") ? TsubasamusuBlueprintEditor::EAccessSpecifier::Private : TsubasamusuBlueprintEditor::EAccessSpecifier::Public;
}

TArray<UBlueprint*> FOptimizeAccessSpecifiersUtility::GetBlueprintsReferenceVariable(const FProperty* InVariable, const UBlueprint* VariableOwnerBlueprint, const bool bExcludeVariableOwnerBlueprint)
{
	TArray<UBlueprint*> BlueprintsReferenceVariable = GetReferencerBlueprints(VariableOwnerBlueprint);
	
	BlueprintsReferenceVariable.RemoveAll([InVariable, VariableOwnerBlueprint](const UBlueprint* InReferencerBlueprint)
	{
		return !IsBlueprintReferencesVariable(InReferencerBlueprint, InVariable, VariableOwnerBlueprint);
	});
	
	if (!bExcludeVariableOwnerBlueprint)
	{
		BlueprintsReferenceVariable.AddUnique(const_cast<UBlueprint*>(VariableOwnerBlueprint));
	}
	
	return BlueprintsReferenceVariable;
}

TArray<UBlueprint*> FOptimizeAccessSpecifiersUtility::GetReferencerBlueprints(const UBlueprint* InReferencedBlueprint)
{
	TArray<UBlueprint*> ReferencerBlueprints;
	
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	
	FARFilter AssetRegistryFilter;
	AssetRegistryModule.Get().GetReferencers(InReferencedBlueprint->GetPackage()->GetFName(), AssetRegistryFilter.PackageNames, UE::AssetRegistry::EDependencyCategory::Package, UE::AssetRegistry::EDependencyQuery::Hard);

	if (AssetRegistryFilter.PackageNames.Num() == 0)
	{
		return ReferencerBlueprints;
	}
	
	GWarn->BeginSlowTask(LOCTEXT("LoadingReferencerAssets", "Loading Referencing Assets ..."), true);
	{
		AssetRegistryFilter.TagsAndValues.Add(FBlueprintTags::IsDataOnly, TOptional<FString>(TEXT("false")));
		TArray<FAssetData> ReferencerAssetDataArray;
		AssetRegistryModule.Get().GetAssets(AssetRegistryFilter, ReferencerAssetDataArray);

		for (const FAssetData& ReferencerAssetData : ReferencerAssetDataArray)
		{
			UObject* ReferencerAsset = ReferencerAssetData.GetAsset();
			UBlueprint* ReferencerBlueprint = Cast<UBlueprint>(ReferencerAsset);

			if (IsValid(ReferencerBlueprint) && ReferencerBlueprint != InReferencedBlueprint)
			{
				ReferencerBlueprints.AddUnique(ReferencerBlueprint);
				continue;
			}
			
			const UWorld* ReferencerWorld = Cast<const UWorld>(ReferencerAsset);
			
			if (IsValid(ReferencerWorld))
			{
				const TObjectPtr<ULevel>& PersistentLevel = ReferencerWorld->PersistentLevel;
				
				if (IsValid(PersistentLevel) && IsValid(PersistentLevel->OwningWorld))
				{
					ULevelScriptBlueprint* PersistentLevelBlueprint = PersistentLevel->GetLevelScriptBlueprint();
				
					if (IsValid(PersistentLevelBlueprint) && PersistentLevelBlueprint != InReferencedBlueprint)
					{
						ReferencerBlueprints.AddUnique(PersistentLevelBlueprint);
						continue;
					}
				}
			}
		}
	}
	
	GWarn->EndSlowTask();
	return ReferencerBlueprints;
}

bool FOptimizeAccessSpecifiersUtility::IsBlueprintReferencesVariable(const UBlueprint* BlueprintToCheck, const FProperty* VariableToCheck, const UBlueprint* VariableOwnerBlueprint)
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
