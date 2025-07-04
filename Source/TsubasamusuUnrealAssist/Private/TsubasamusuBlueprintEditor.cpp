// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "TsubasamusuBlueprintEditor.h"

void FTsubasamusuBlueprintEditor::InitTsubasamusuBlueprintEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, const TArray<UBlueprint*>& InBlueprints, bool bShouldOpenInDefaultsMode)
{
	InitBlueprintEditor(Mode, InitToolkitHost, InBlueprints, bShouldOpenInDefaultsMode);
}