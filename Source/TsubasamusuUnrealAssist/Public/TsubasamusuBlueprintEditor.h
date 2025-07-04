// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "BlueprintEditor.h"
#include "CoreMinimal.h"

class FTsubasamusuBlueprintEditor : public FBlueprintEditor
{
public:
	
	void InitTsubasamusuBlueprintEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, const TArray<UBlueprint*>& InBlueprints, bool bShouldOpenInDefaultsMode);

	TWeakPtr<SGraphEditor> GetFocusedGraphEditor() const;
};
