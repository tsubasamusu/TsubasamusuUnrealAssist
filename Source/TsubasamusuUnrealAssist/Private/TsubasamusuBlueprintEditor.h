// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "BlueprintEditor.h"
#include "CoreMinimal.h"

class FTsubasamusuBlueprintEditor final : public FBlueprintEditor
{
public:
	void InitTsubasamusuBlueprintEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, const TArray<UBlueprint*>& InBlueprints, bool bShouldOpenInDefaultsMode);
	TWeakPtr<SGraphEditor> GetFocusedGraphEditor() const;
	
protected:
	virtual void CreateDefaultCommands() override;
	
private:
	void RegisterOriginalMenus() const;
	void ChangeMemberVariablesAccessSpecifierToPrivate_OnClicked();
	
	static TArray<FProperty*> GetMemberVariables(const UBlueprint* InBlueprint);
	static bool IsBlueprintReferencesVariable(const UBlueprint* BlueprintToCheck, const FProperty* VariableToCheck, const UBlueprint* VariableOwnerBlueprint);
};
