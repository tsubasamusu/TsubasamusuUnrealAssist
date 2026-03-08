// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Type/TsubasamusuUnrealAssistMacros.h"

class FRecommendedEditorSettingsApplier final
{
public:
	static void ApplyRecommendedEditorSettings();

private:
	static void ApplyRecommendedLoadingSavingSettings();
	static void ApplyRecommendedStyleSettings();
	static void ApplyRecommendedGraphEditorSettings();
	static void ApplyRecommendedInternationalizationSettings();
	static void ApplyRecommendedBlueprintEditorSettings();
	static void ApplyRecommendedLevelEditorPlaySettings();
	
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 1, 0)
	static void ApplyRecommendedOutputLogSettings();
#endif
};
