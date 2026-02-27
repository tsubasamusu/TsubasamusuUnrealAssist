// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class FBlueprintMemberUtility final
{
public:
	static UEdGraph* FindFunctionGraph(const FName& InFunctionName, const UBlueprint* InBlueprint);
};
