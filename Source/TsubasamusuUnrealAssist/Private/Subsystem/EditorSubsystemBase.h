// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

template<typename DerivedClass>
class TEditorSubsystemBase
{
public:
	static DerivedClass* GetChecked()
	{
		DerivedClass* EditorSubsystem = GEditor->GetEditorSubsystem<DerivedClass>();
		check(IsValid(EditorSubsystem));
		return EditorSubsystem;
	}
};
