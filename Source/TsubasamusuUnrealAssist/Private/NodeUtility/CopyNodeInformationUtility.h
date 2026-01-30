// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class FCopyNodeInformationUtility final
{
public:
	static void AddCopyNodeInformationMenu(const TWeakObjectPtr<UEdGraph> InGraph, FMenuBuilder& InMenuBuilder);
};
