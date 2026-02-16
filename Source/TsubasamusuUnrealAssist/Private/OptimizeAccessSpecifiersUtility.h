// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "BlueprintEditor.h"
#include "CoreMinimal.h"

namespace TsubasamusuBlueprintEditor
{
	enum class EAccessSpecifier : uint8
	{
		None,
		Private,
		Protected,
		Public
	};
	
	enum class EMemberType
	{
		None,
		Variable,
		Function,
		CustomEvent
	};
}

struct FAccessSpecifierOptimizationRow
{
public:
	explicit FAccessSpecifierOptimizationRow(
		const FName& InMemberName = NAME_None,
		const TsubasamusuBlueprintEditor::EMemberType InMemberType = TsubasamusuBlueprintEditor::EMemberType::None,
		const TsubasamusuBlueprintEditor::EAccessSpecifier InCurrentAccessSpecifier = TsubasamusuBlueprintEditor::EAccessSpecifier::None,
		const TsubasamusuBlueprintEditor::EAccessSpecifier InRecommendedAccessSpecifier = TsubasamusuBlueprintEditor::EAccessSpecifier::None,
		const bool bInSelected = true)
		: MemberName(InMemberName),
		  MemberType(InMemberType),
		  CurrentAccessSpecifier(InCurrentAccessSpecifier),
		  RecommendedAccessSpecifier(InRecommendedAccessSpecifier),
		  bSelected(bInSelected) {}
	
	FName MemberName;
	TsubasamusuBlueprintEditor::EMemberType MemberType;
	TsubasamusuBlueprintEditor::EAccessSpecifier CurrentAccessSpecifier;
	TsubasamusuBlueprintEditor::EAccessSpecifier RecommendedAccessSpecifier;
	bool bSelected;
	TSharedPtr<SCheckBox> CheckBox;
};

class FOptimizeAccessSpecifiersUtility final
{
public:
	static void OnBlueprintEditorOpened(UBlueprint* InOpenedBlueprint);
	
private:
	static void RegisterAdditionalMenus(const TSharedPtr<FBlueprintEditor> InBlueprintEditor);
	static void OnOptimizeAccessSpecifiersClicked(const TSharedPtr<FBlueprintEditor> InBlueprintEditor);
	
	static TArray<FProperty*> GetVariables(const UBlueprint* InBlueprint);
	static void RemoveVariablesShouldNotBePrivate(TArray<FProperty*>& OutVariables, const UBlueprint* VariablesOwnerBlueprint);

	static TsubasamusuBlueprintEditor::EAccessSpecifier GetOptimalAccessSpecifier(const FProperty* InVariable, const UBlueprint* VariableOwnerBlueprint);
	static TsubasamusuBlueprintEditor::EAccessSpecifier GetCurrentAccessSpecifier(const FProperty* InVariable, const UBlueprint* VariableOwnerBlueprint);
	
	static TArray<UBlueprint*> GetBlueprintsReferenceVariable(const FProperty* InVariable, const UBlueprint* VariableOwnerBlueprint, const bool bExcludeVariableOwnerBlueprint = true);
	static TArray<UBlueprint*> GetReferencerBlueprints(const UBlueprint* InReferencedBlueprint);
	
	static bool IsBlueprintReferencesVariable(const UBlueprint* BlueprintToCheck, const FProperty* VariableToCheck, const UBlueprint* VariableOwnerBlueprint);
};
