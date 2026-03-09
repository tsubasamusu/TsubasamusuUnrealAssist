// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "BlueprintEditorUtility.h"
#include "BlueprintCommandContext.h"
#include "BlueprintEditor.h"
#include "Algo/AnyOf.h"
#include "Toolkits/ToolkitManager.h"
#include "Type/TsubasamusuUnrealAssistMacros.h"

#if UE_VERSION_OLDER_THAN(5, 1, 0)
#include "ToolMenus.h"
#endif

#define LOCTEXT_NAMESPACE "FCommandUtility"

void FBlueprintEditorUtility::RegisterCommandInBlueprintEditMenu(const FBlueprintCommandContext& InBlueprintCommandContext)
{
	if (InBlueprintCommandContext.IsValid())
	{
		const TWeakPtr<FBlueprintEditor> WeakBlueprintEditor = GetBlueprintEditor(InBlueprintCommandContext.Blueprint);
		const TArray<FCanExecuteAction> AdditionalConditionsToExecuteAction = InBlueprintCommandContext.AdditionalConditionsToExecuteAction;
		const TArray<FName> TargetModes = InBlueprintCommandContext.TargetModes;

		auto CanExecuteAction = [WeakBlueprintEditor, AdditionalConditionsToExecuteAction, TargetModes]()
		{
			if (WeakBlueprintEditor.IsValid())
			{
				TSharedPtr<FBlueprintEditor> PinnedBlueprintEditor = WeakBlueprintEditor.Pin();
				auto IsModeMatchesTargetBlueprint = [WeakBlueprintEditor, PinnedBlueprintEditor](const FName& InTargetMode)
				{
					return PinnedBlueprintEditor->IsModeCurrent(InTargetMode);
				};
		
				if (Algo::AnyOf(TargetModes, IsModeMatchesTargetBlueprint))
				{
					for (const FCanExecuteAction& AdditionalConditionToExecuteAction : AdditionalConditionsToExecuteAction)
					{
						if (!AdditionalConditionToExecuteAction.Execute())
						{
							return false;
						}
					}
					
					return true;
				}
			}
			
			return false;
		};
		
		const TSharedPtr<FBlueprintEditor> PinnedBlueprintEditor = WeakBlueprintEditor.Pin();
		const TSharedPtr<FUICommandList> ToolkitCommands = PinnedBlueprintEditor->GetToolkitCommands();
		
		ToolkitCommands->MapAction(InBlueprintCommandContext.UICommandInfo, InBlueprintCommandContext.ExecuteAction, FCanExecuteAction::CreateLambda(CanExecuteAction));
		
		const FName EditMenuName = *(PinnedBlueprintEditor->GetToolMenuName().ToString() + TEXT(".Edit"));
		const FName ParentEditMenuName = TEXT("MainFrame.MainMenu.Edit");

		UToolMenu* ToolMenu = UToolMenus::Get()->RegisterMenu(EditMenuName, ParentEditMenuName, EMultiBoxType::Menu, false);

		const FName SectionName = TEXT("EditTsubasamusuUnrealAssist");
		const FText SectionLabel = LOCTEXT("EditMenu_TsubasamusuUnrealAssist", "Tsubasamusu Unreal Assist");
		const FName AboveSectionName = TEXT("EditSearch");

		FToolMenuSection& ToolMenuSection = ToolMenu->AddSection(SectionName, SectionLabel);
		ToolMenuSection.InsertPosition = FToolMenuInsert(AboveSectionName, EToolMenuInsertType::After);

		ToolMenuSection.AddMenuEntry(InBlueprintCommandContext.UICommandInfo);
	}
}

TSharedPtr<FBlueprintEditor> FBlueprintEditorUtility::GetBlueprintEditor(const UBlueprint* InBlueprint)
{
	if (IsValid(InBlueprint))
	{
		const TSharedPtr<IToolkit> Toolkit = FToolkitManager::Get().FindEditorForAsset(InBlueprint);
		return StaticCastSharedPtr<FBlueprintEditor>(Toolkit);
	}
	
	return nullptr;
}

#undef LOCTEXT_NAMESPACE
