// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

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
	
#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1)
	static void ApplyRecommendedOutputLogSettings();
#endif
};
