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

class FTsubasamusuBlueprintEditor final : public FBlueprintEditor
{
public:
	void InitTsubasamusuBlueprintEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, const TArray<UBlueprint*>& InBlueprints, bool bShouldOpenInDefaultsMode);
	TWeakPtr<SGraphEditor> GetFocusedGraphEditor() const;
	
protected:
	virtual void CreateDefaultCommands() override;
	
private:
	void RegisterAdditionalMenus() const;
	void OptimizeAccessSpecifiers_OnClicked();
	
	static TArray<FProperty*> GetVariables(const UBlueprint* InBlueprint);
	static void RemoveVariablesShouldNotBePrivate(TArray<FProperty*>& OutVariables, const UBlueprint* VariablesOwnerBlueprint);

	static TsubasamusuBlueprintEditor::EAccessSpecifier GetOptimalAccessSpecifier(const FProperty* InVariable, const UBlueprint* VariableOwnerBlueprint);
	
	static TArray<UBlueprint*> GetBlueprintsReferenceVariable(const FProperty* InVariable, const UBlueprint* VariableOwnerBlueprint, const bool bExcludeVariableOwnerBlueprint = true);
	static TArray<UBlueprint*> GetReferencerBlueprints(const UBlueprint* InReferencedBlueprint);
	
	static bool IsBlueprintReferencesVariable(const UBlueprint* BlueprintToCheck, const FProperty* VariableToCheck, const UBlueprint* VariableOwnerBlueprint);
};
