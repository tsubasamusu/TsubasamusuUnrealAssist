// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "TsubasamusuBlueprintEditor.h"
#include "TsubasamusuBlueprintEditorCommands.h"

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
}

void FTsubasamusuBlueprintEditor::RegisterOriginalMenus() const
{
	const FName EditMenuName = *(GetToolMenuName().ToString() + TEXT(".Edit"));
	const FName ParentEditMenuName = TEXT("MainFrame.MainMenu.Edit");
	
	UToolMenu* ToolMenu = UToolMenus::Get()->RegisterMenu(EditMenuName, ParentEditMenuName);
	
	const FName SectionName = TEXT("EditTsubasamusuUnrealAssist");
	const FText SectionLabel = LOCTEXT("EditMenu_TsubasamusuUnrealAssist", "Tsubasamusu Unreal Assist");
	const FName AboveSectionName = TEXT("EditSearch");
	
	FToolMenuSection& ToolMenuSection = ToolMenu->AddSection(SectionName, SectionLabel);
	ToolMenuSection.InsertPosition = FToolMenuInsert(AboveSectionName, EToolMenuInsertType::After);
	
	ToolMenuSection.AddMenuEntry(FTsubasamusuBlueprintEditorCommands::Get().ChangeMemberVariablesAccessSpecifierToPrivate);
}

#undef LOCTEXT_NAMESPACE
