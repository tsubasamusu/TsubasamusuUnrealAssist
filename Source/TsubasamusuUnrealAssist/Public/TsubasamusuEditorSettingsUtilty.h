// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class TSUBASAMUSUUNREALASSIST_API FTsubasamusuEditorSettingsUtilty
{
public:
	static void SetupEditorSettingsForTsubasamusu();

	static void SetupLoadingSavingSettings();
	static void SetupStyleSettings();
	static void SetupGraphSettings();
	static void SetupInternationalizationSettings();
};