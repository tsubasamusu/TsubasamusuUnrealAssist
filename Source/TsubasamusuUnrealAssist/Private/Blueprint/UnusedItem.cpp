// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "UnusedItem.h"

#define LOCTEXT_NAMESPACE "FUnusedItem"

void FUnusedItem::AddReferencedObjects(FReferenceCollector& InReferenceCollector)
{
	
}

void FUnusedItem_FunctionGraph::AddReferencedObjects(FReferenceCollector& InReferenceCollector)
{
	InReferenceCollector.AddReferencedObject(FunctionGraph);
}

FText FUnusedItem_FunctionGraph::GetItemNameText() const
{
	if (IsValid(FunctionGraph))
	{
		return FText::FromString(FunctionGraph->GetName());
	}
	
	return FText::GetEmpty();
}

FText FUnusedItem_LocalVariable::GetItemNameText() const
{
	return FText::FromName(LocalVariable.VarName);
}

#undef LOCTEXT_NAMESPACE
