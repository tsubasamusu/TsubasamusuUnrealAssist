// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class FWidgetUtility final
{
public:
	static TSharedPtr<SWidget> GetHoveredWidget();
	static bool IsNodeSelectionWidget(const TSharedPtr<SWidget> InWidget);
};
