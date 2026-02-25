#pragma once

#include "CoreMinimal.h"

class FEditorSettingsUtility final
{
public:
	template<typename SettingsClass>
	static SettingsClass* GetSettingsChecked()
	{
		static_assert(TIsDerivedFrom<SettingsClass, UObject>::Value, "SettingsClass must be derived from UObject.");

		SettingsClass* MutableDefault = GetMutableDefault<SettingsClass>();
		check(IsValid(MutableDefault));

		return MutableDefault;
	}
};
