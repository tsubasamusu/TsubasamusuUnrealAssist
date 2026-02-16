#pragma once

#include "CoreMinimal.h"

class FEditorSettingsUtility final
{
public:
	template<typename T>
	FORCEINLINE static T* GetSettingsChecked()
	{
		static_assert(TIsDerivedFrom<T, UObject>::Value, "T must be a subclass of UObject.");

		T* MutableDefault = GetMutableDefault<T>();
		check(IsValid(MutableDefault));

		return MutableDefault;
	}
};
