// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "TsubasamusuBlueprintEditor.h"
#include "TsubasamusuBlueprintEditorCommands.h"
#include "Components/TimelineComponent.h"
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

#undef LOCTEXT_NAMESPACE
