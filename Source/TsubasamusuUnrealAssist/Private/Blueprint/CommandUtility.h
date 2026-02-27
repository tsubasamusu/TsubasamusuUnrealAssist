// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class FBlueprintCommandContext;

class FCommandUtility final
{
public:
	static void RegisterCommandInBlueprintEditMenu(const FBlueprintCommandContext& InBlueprintCommandContext);
};
