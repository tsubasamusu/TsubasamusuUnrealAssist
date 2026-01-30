// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class FTsubasamusuEditorSettingsUtility final
{
public:
	static void SetupEditorSettingsForTsubasamusu();

	static void SetupLoadingSavingSettings();
	static void SetupStyleSettings();
	static void SetupGraphEditorSettings();
	static void SetupInternationalizationSettings();
	static void SetupBlueprintEditorSettings();
	static void SetupOutputLogSettings();

	template<typename T>
	static T* GetSettingsChecked()
	{
		static_assert(TIsDerivedFrom<T, UObject>::Value, "T must be a subclass of UObject.");

		T* MutableDefault = GetMutableDefault<T>();

		const UObject* Settings = Cast<UObject>(MutableDefault);

		check(IsValid(Settings));

		return MutableDefault;
	}
};
