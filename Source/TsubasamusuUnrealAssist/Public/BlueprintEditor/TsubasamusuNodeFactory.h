// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"

class FNodeSuggester;

class TSUBASAMUSUUNREALASSIST_API FTsubasamusuNodeFactory final : public FGraphPanelNodeFactory
{
public:
	
	virtual TSharedPtr<SGraphNode> CreateNode(UEdGraphNode* Node) const override;

private:

	mutable TSharedPtr<FNodeSuggester> NodeSuggester;
};
