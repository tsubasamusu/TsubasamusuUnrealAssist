// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "TsubasamusuBlueprintEditor.h"

void FTsubasamusuBlueprintEditor::InitTsubasamusuBlueprintEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, const TArray<UBlueprint*>& InBlueprints, bool bShouldOpenInDefaultsMode)
{
	InitBlueprintEditor(Mode, InitToolkitHost, InBlueprints, bShouldOpenInDefaultsMode);
}

TWeakPtr<SGraphEditor> FTsubasamusuBlueprintEditor::GetFocusedGraphEditor() const
{
	return FocusedGraphEdPtr;
}
