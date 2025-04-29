// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"

class TSUBASAMUSUUNREALASSIST_API FPlayerCharacterNodeFactory : public FGraphPanelNodeFactory
{
	virtual TSharedPtr<SGraphNode> CreateNode(class UEdGraphNode* Node) const override;
};
