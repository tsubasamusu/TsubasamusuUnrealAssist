// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class TSUBASAMUSUUNREALASSIST_API FCopyNodeInformationUtility final
{
public:
	
	static void AddCopyNodeInformationMenu(const TWeakObjectPtr<UEdGraph> InGraph, FMenuBuilder& InMenuBuilder);
};
