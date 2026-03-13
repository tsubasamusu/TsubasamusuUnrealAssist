// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "AccessSpecifierOptimizer.h"
#include "BlueprintCommandContext.h"
#include "BlueprintMember.h"
#include "BlueprintMemberUtility.h"
#include "BlueprintEditorUtility.h"
#include "Slate/SAccessSpecifierOptimizationRow.h"
#include "Command/TsubasamusuBlueprintEditorCommands.h"
#include "Algo/AnyOf.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/TimelineComponent.h"
#include "Debug/EditorMessageUtility.h"
#include "Engine/LevelScriptBlueprint.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Type/TsubasamusuUnrealAssistStructs.h"
#include "K2Node_FunctionEntry.h"
#include "Type/TsubasamusuUnrealAssistMacros.h"

#if EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
#include "K2Node_CustomEvent.h"
#endif

#if UE_VERSION_OLDER_THAN(5, 3, 0)
#include "Misc/FeedbackContext.h"
#endif

#if UE_VERSION_OLDER_THAN(5, 4, 0)
#include "Misc/ScopedSlowTask.h"
#endif

#define LOCTEXT_NAMESPACE "FAccessSpecifierOptimizer"

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

void FAccessSpecifierOptimizer::RegisterOptimizeAccessSpecifiersMenu(UBlueprint* InBlueprint)
{
	const FBlueprintCommandContext BlueprintCommandContext(FTsubasamusuBlueprintEditorCommands::Get().OptimizeAccessSpecifiers,
		FExecuteAction::CreateStatic(&OnOptimizeAccessSpecifiersClicked, InBlueprint), InBlueprint);
	
	FBlueprintEditorUtility::RegisterCommandInBlueprintEditMenu(BlueprintCommandContext);
}

void FAccessSpecifierOptimizer::OnOptimizeAccessSpecifiersClicked(UBlueprint* InBlueprint)
{
	if (!IsValid(InBlueprint))
	{
		return;
	}
	
	TSharedPtr<TArray<TSharedPtr<FBlueprintMember>>> Members = GetMembers(InBlueprint);
	
	if (!Members.IsValid() || Members->IsEmpty())
	{
		const FText NotificationText = LOCTEXT("NoMembersToCheck", "No members to check for.");
		FEditorMessageUtility::DisplaySimpleNotification(NotificationText);
		return;
	}
	
	// Remove members that already have the optimal access specifier set
	Members->RemoveAll([](const TSharedPtr<FBlueprintMember> InMember)
	{
		return InMember->GetCurrentAccessSpecifier() == InMember->GetOptimalAccessSpecifier();
	});
	
	if (Members->IsEmpty())
	{
		const FText NotificationText = LOCTEXT("NoMembersShouldChangeAccessSpecifier", "There are no members that should change access specifier.");
		FEditorMessageUtility::DisplaySimpleNotification(NotificationText);
		return;
	}

	const TSharedPtr<TArray<TSharedPtr<FAccessSpecifierOptimizationRow>>> RowItems = MakeShared<TArray<TSharedPtr<FAccessSpecifierOptimizationRow>>>();
	
	for (const TSharedPtr<FBlueprintMember> Member : *Members)
	{
		RowItems->Add(Member->GetAccessSpecifierOptimizationRow());
	}
	
	const FName CheckBoxColumnId = TEXT("CheckBox");
	const FName MemberNameColumnId = TEXT("MemberName");
	const FName CurrentAccessSpecifierColumnId = TEXT("CurrentAccessSpecifier");
	const FName OptimalAccessSpecifierColumnId = TEXT("OptimalAccessSpecifier");
	
	const TSharedRef<SListView<TSharedPtr<FAccessSpecifierOptimizationRow>>> DialogContent = SNew(SListView<TSharedPtr<FAccessSpecifierOptimizationRow>>)
		.ListItemsSource(RowItems.Get())
		.SelectionMode(ESelectionMode::Type::None)
		.HeaderRow
		(
			SNew(SHeaderRow)
			+ SHeaderRow::Column(CheckBoxColumnId)
			.FixedWidth(30.f)
			[
				SNew(SBox)
				.HAlign(HAlign_Center)
				[
					SNew(SCheckBox)
					.IsChecked_Lambda([Members]()
					{
						bool bFoundCheckedRowItem = false;
						// ReSharper disable once CppTooWideScope
						bool bFoundUncheckedRowItem = false;
								
						if (Members.IsValid())
						{
							for (const TSharedPtr<FBlueprintMember> Member : *Members)
							{
								if (Member->AccessSpecifierOptimizationRowWidget.IsValid())
								{
									if (Member->AccessSpecifierOptimizationRowWidget->IsChecked())
									{
										bFoundCheckedRowItem = true;
									}
									else
									{
										bFoundUncheckedRowItem = true;
									}
								}

								if (bFoundCheckedRowItem && bFoundUncheckedRowItem)
								{
									return ECheckBoxState::Undetermined;
								}
							}
						}

						return bFoundCheckedRowItem ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
					})
					.OnCheckStateChanged_Lambda([Members](const ECheckBoxState InNewState)
					{
						if (InNewState != ECheckBoxState::Undetermined)
						{
							for (const TSharedPtr<FBlueprintMember> Member : *Members)
							{
								if (Member->AccessSpecifierOptimizationRowWidget.IsValid() && Member->AccessSpecifierOptimizationRowWidget->GetCheckedState() != InNewState)
								{
							  		Member->AccessSpecifierOptimizationRowWidget->ToggleCheckedState();
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
			+ SHeaderRow::Column(OptimalAccessSpecifierColumnId)
			.DefaultLabel(LOCTEXT("HeaderLabel_OptimalAccessSpecifier", "Optimal"))
			.HAlignHeader(HAlign_Center)
			.FixedWidth(80.f)
		)
		.OnGenerateRow_Lambda([CheckBoxColumnId, MemberNameColumnId, CurrentAccessSpecifierColumnId, OptimalAccessSpecifierColumnId, Members](const TSharedPtr<FAccessSpecifierOptimizationRow> InRowItem, const TSharedRef<STableViewBase>& InOwnerTableView)
		{
			TSharedRef<SAccessSpecifierOptimizationRow> AccessSpecifierOptimizationRowWidget = SNew(SAccessSpecifierOptimizationRow, InOwnerTableView)
				.RowItem(InRowItem)
				.CheckBoxColumnId(CheckBoxColumnId)
				.MemberNameColumnId(MemberNameColumnId)
				.CurrentAccessSpecifierColumnId(CurrentAccessSpecifierColumnId)
				.OptimalAccessSpecifierColumnId(OptimalAccessSpecifierColumnId);
			
			const TSharedPtr<FBlueprintMember>* Member = Members->FindByPredicate([InRowItem](const TSharedPtr<FBlueprintMember> InMember)
			{
				return InMember->GetMemberName() == InRowItem->MemberName;
			});
			
			check(Member && Member->IsValid())
			(*Member)->AccessSpecifierOptimizationRowWidget = AccessSpecifierOptimizationRowWidget;
			
			return AccessSpecifierOptimizationRowWidget;
		});

	auto IsCheckedMember = [](const TSharedPtr<FBlueprintMember> InMember)
	{
		return InMember->AccessSpecifierOptimizationRowWidget.IsValid() && InMember->AccessSpecifierOptimizationRowWidget->IsChecked();
	};

	const TAttribute<bool> OkButtonIsEnabled = TAttribute<bool>::CreateLambda([Members, &IsCheckedMember]()
	{
		return Algo::AnyOf(*Members, IsCheckedMember);
	});
	
	const FText DialogTitle = LOCTEXT("OptimizeAccessSpecifiersDialog_Title", "Optimize Access Specifiers");
	const FText DialogMessage = LOCTEXT("OptimizeAccessSpecifiersDialog_Message", "You might want to change the access specifiers for these members.");
	const FText ApplyButtonText = LOCTEXT("OptimizeAccessSpecifiersDialog_ApplyButton", "Apply Optimal Access Specifiers");
	const FText CancelButtonText = LOCTEXT("OptimizeAccessSpecifiersDialog_CancelButton", "Cancel");

	const ETsubasamusuDialogButton PressedButton = FEditorMessageUtility::ShowCustomDialog(DialogTitle, DialogMessage, ApplyButtonText, CancelButtonText, DialogContent, OkButtonIsEnabled);
	
	if (PressedButton != ETsubasamusuDialogButton::OK || !Algo::AnyOf(*Members, IsCheckedMember))
	{
		return;
	}
	
	for (const TSharedPtr<FBlueprintMember> Member : *Members)
	{
		if (IsCheckedMember(Member))
		{
			const ETsubasamusuAccessSpecifier OptimalAccessSpecifier = Member->GetOptimalAccessSpecifier();
			Member->SetAccessSpecifier(OptimalAccessSpecifier);
		}
	}
	
	const FText NotificationText = LOCTEXT("SuccessfullyAppliedOptimalAccessSpecifiers", "Successfully applied optimal access specifiers.");
	FEditorMessageUtility::DisplaySimpleNotification(NotificationText, SNotificationItem::ECompletionState::CS_Success);
}

TSharedPtr<TArray<TSharedPtr<FBlueprintMember>>> FAccessSpecifierOptimizer::GetMembers(UBlueprint* InBlueprint)
{
	TSharedPtr<TArray<TSharedPtr<FBlueprintMember>>> Members = MakeShared<TArray<TSharedPtr<FBlueprintMember>>>();
	const TArray<TObjectPtr<const UBlueprint>> ReferencerBlueprints = GetReferencerBlueprints(InBlueprint);
	
	// Variables
	{
		const TArray<FProperty*> Variables = FBlueprintMemberUtility::GetVariables(InBlueprint);
		
		for (FProperty* Variable : Variables)
		{
			TSharedPtr<FBlueprintMember> Member = MakeShared<FBlueprintMember_Variable>(InBlueprint, ReferencerBlueprints, Variable);
			Member->Initialize();
			Members->Add(Member);
		}
	}
	
	// Functions
	{
		const TMap<UFunction*, UK2Node_FunctionEntry*> Functions = GetFunctions(InBlueprint);
		
		for (TPair<UFunction*, UK2Node_FunctionEntry*> Function : Functions)
		{
			TSharedPtr<FBlueprintMember> Member = MakeShared<FBlueprintMember_Function>(InBlueprint, ReferencerBlueprints, Function.Key, Function.Value);
			Member->Initialize();
			Members->Add(Member);
		}
	}
	
#if EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
	// Events
	{
		const TMap<UFunction*, UK2Node_CustomEvent*> Events = GetEvents(InBlueprint);
		
		for (TPair<UFunction*, UK2Node_CustomEvent*> Event : Events)
		{
			TSharedPtr<FBlueprintMember> Member = MakeShared<FBlueprintMember_Event>(InBlueprint, ReferencerBlueprints, Event.Key, Event.Value);
			Member->Initialize();
			Members->Add(Member);
		}
	}
#endif
	
	return Members;
}

TMap<UFunction*, UK2Node_FunctionEntry*> FAccessSpecifierOptimizer::GetFunctions(UBlueprint* InBlueprint)
{
	if (IsValid(InBlueprint))
	{
		auto FunctionToFindGraph = [](const FName& InFunctionName, UBlueprint* InBlueprintToFindGraph) -> UEdGraph*
		{
			return FBlueprintMemberUtility::FindFunctionGraph(InFunctionName, InBlueprintToFindGraph);
		};
		
		auto FunctionToCheckEditablePinNode = [](const FName& /*InFunctionOrEventName*/, const UK2Node_EditablePinBase* InEditablePinNode)
		{
			return FBlueprintMemberUtility::IsFunctionEntryNode(InEditablePinNode);
		};
	
		return GetFunctionBaseMembers<decltype(FunctionToFindGraph), decltype(FunctionToCheckEditablePinNode), UK2Node_FunctionEntry>(InBlueprint, FunctionToFindGraph, FunctionToCheckEditablePinNode);
	}
	
	return TMap<UFunction*, UK2Node_FunctionEntry*>();
}

#if EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
TMap<UFunction*, UK2Node_CustomEvent*> FAccessSpecifierOptimizer::GetEvents(UBlueprint* InBlueprint)
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

TArray<TObjectPtr<const UBlueprint>> FAccessSpecifierOptimizer::GetReferencerBlueprints(const UBlueprint* InReferencedBlueprint)
{
	TArray<TObjectPtr<const UBlueprint>> ReferencerBlueprints;
	
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	
	FARFilter AssetRegistryFilter;
	AssetRegistryModule.Get().GetReferencers(InReferencedBlueprint->GetPackage()->GetFName(), AssetRegistryFilter.PackageNames, UE::AssetRegistry::EDependencyCategory::Package, UE::AssetRegistry::EDependencyQuery::Hard);

	if (AssetRegistryFilter.PackageNames.Num() == 0)
	{
		return ReferencerBlueprints;
	}
	
	AssetRegistryFilter.TagsAndValues.Add(FBlueprintTags::IsDataOnly, TOptional<FString>(TEXT("false")));
	TArray<FAssetData> ReferencerAssetDataArray;
	AssetRegistryModule.Get().GetAssets(AssetRegistryFilter, ReferencerAssetDataArray);

	FScopedSlowTask ScopedSlowTask(ReferencerAssetDataArray.Num(), LOCTEXT("CheckReferencerAssets", "Check Referencer Assets..."));
	ScopedSlowTask.MakeDialog();
	
	for (const FAssetData& ReferencerAssetData : ReferencerAssetDataArray)
	{
		ScopedSlowTask.EnterProgressFrame(1.f, FText::Format(LOCTEXT("CheckReferencerAssetsProgress", "Check Referencer Assets: {0}"), FText::FromName(ReferencerAssetData.AssetName)));
	
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
	
	return ReferencerBlueprints;
}

#undef LOCTEXT_NAMESPACE
