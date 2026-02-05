// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class FTsubasamusuEditorSettingsUtility final
{
public:
	static void ChangeEditorSettingsForTsubasamusu();

	template<typename T>
	FORCEINLINE static T* GetSettingsChecked()
	{
		static_assert(TIsDerivedFrom<T, UObject>::Value, "T must be a subclass of UObject.");

		T* MutableDefault = GetMutableDefault<T>();
		check(IsValid(MutableDefault));

		return MutableDefault;
	}
	
private:
	static void ChangeLoadingSavingSettingsForTsubasamusu();
	static void ChangeStyleSettingsForTsubasamusu();
	static void ChangeGraphEditorSettingsForTsubasamusu();
	static void ChangeInternationalizationSettingsForTsubasamusu();
	static void ChangeBlueprintEditorSettingsForTsubasamusu();
	static void ChangeOutputLogSettingsForTsubasamusu();
	static void ChangeLevelEditorPlaySettingsForTsubasamusu();
};
