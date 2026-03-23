// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "UnusedItem.h"

FText FUnusedItem_Function::GetItemNameText() const
{
	return FunctionGraph.IsValid() ? FText::FromString(FunctionGraph->GetName()) : FText::GetEmpty();
}

FText FUnusedItem_LocalVariable::GetItemNameText() const
{
	return FText::FromName(LocalVariable.VarName);
}
