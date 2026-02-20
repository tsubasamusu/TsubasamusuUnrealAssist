// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class FSelectedNodeMenuExtender final
{
public:
	static void RegisterSelectedNodeMenu();

private:
	static TSharedRef<FExtender> ExtendSelectedNodeMenu(TSharedRef<FUICommandList> InUiCommandList, const UEdGraph* InGraph, const UEdGraphNode* InNode, const UEdGraphPin* InPin, bool bIsDebugging);
};
