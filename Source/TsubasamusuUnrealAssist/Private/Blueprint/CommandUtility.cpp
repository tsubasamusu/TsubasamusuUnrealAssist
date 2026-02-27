// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "CommandUtility.h"
#include "BlueprintCommandContext.h"
#include "BlueprintEditor.h"
#include "Algo/AnyOf.h"
#include "Toolkits/ToolkitManager.h"

#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 0)
#include "ToolMenus.h"
#endif

#define LOCTEXT_NAMESPACE "FCommandUtility"

void FCommandUtility::RegisterCommandInBlueprintEditMenu(const FBlueprintCommandContext& InBlueprintCommandContext)
{
	if (InBlueprintCommandContext.IsValid())
	{
		const TSharedPtr<IToolkit> Toolkit = FToolkitManager::Get().FindEditorForAsset(InBlueprintCommandContext.Blueprint);
		
		if (Toolkit.IsValid())
		{
			const TSharedPtr<FBlueprintEditor> BlueprintEditor = StaticCastSharedPtr<FBlueprintEditor>(Toolkit);
	
			auto CanExecuteAction = [BlueprintEditor, InBlueprintCommandContext]()
			{
				auto IsModeMatchesTargetBlueprint = [BlueprintEditor](const FName& InTargetMode)
				{
					return BlueprintEditor->IsModeCurrent(InTargetMode);
				};
		
				return Algo::AnyOf(InBlueprintCommandContext.TargetModes, IsModeMatchesTargetBlueprint);
			};
	
			const TSharedPtr<FUICommandList> ToolkitCommands = BlueprintEditor->GetToolkitCommands();
			ToolkitCommands->MapAction(InBlueprintCommandContext.UICommandInfo, InBlueprintCommandContext.ExecuteAction, FCanExecuteAction::CreateLambda(CanExecuteAction));
			
			const FName EditMenuName = *(BlueprintEditor->GetToolMenuName().ToString() + TEXT(".Edit"));
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
}

#undef LOCTEXT_NAMESPACE
