// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class FUnusedItem : public FGCObject
{
public:
	FUnusedItem(){}
	
	//~ Begin FGCObject Interface
	virtual void AddReferencedObjects(FReferenceCollector& InReferenceCollector) override;
	virtual FString GetReferencerName() const override
	{
		return TEXT("FUnusedItem");
	}
	//~ End FGCObject Interface
	
	virtual FText GetItemNameText() const = 0;
};

class FUnusedItem_FunctionGraph final : public FUnusedItem
{
public:
	explicit FUnusedItem_FunctionGraph(const TObjectPtr<UEdGraph> InFunctionGraph)
		: FunctionGraph(InFunctionGraph){}
	
	//~ Begin FGCObject Interface
	virtual void AddReferencedObjects(FReferenceCollector& InReferenceCollector) override;
	//~ End FGCObject Interface
	
	//~ Begin FUnusedItem Interface
	virtual FText GetItemNameText() const override;
	//~ End FUnusedItem Interface
	
	TObjectPtr<UEdGraph> FunctionGraph;
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
