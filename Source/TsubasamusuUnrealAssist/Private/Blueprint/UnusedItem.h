// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class FUnusedItem
{
public:
	FUnusedItem(){}
	virtual ~FUnusedItem() = default;
	
	virtual FText GetItemNameText() const = 0;
};

class FUnusedItem_Function final : public FUnusedItem
{
public:
	explicit FUnusedItem_Function(const TWeakObjectPtr<UEdGraph> InFunctionGraph)
		: FunctionGraph(InFunctionGraph){}
	
	//~ Begin FUnusedItem Interface
	virtual FText GetItemNameText() const override;
	//~ End FUnusedItem Interface
	
	TWeakObjectPtr<UEdGraph> FunctionGraph;
};

class FUnusedItem_LocalVariable final : public FUnusedItem
{
public:
	explicit FUnusedItem_LocalVariable(const FBPVariableDescription& InLocalVariable)
		: LocalVariable(InLocalVariable){}
	
	//~ Begin FUnusedItem Interface
	virtual FText GetItemNameText() const override;
	//~ End FUnusedItem Interface
	
	FBPVariableDescription LocalVariable;
};
