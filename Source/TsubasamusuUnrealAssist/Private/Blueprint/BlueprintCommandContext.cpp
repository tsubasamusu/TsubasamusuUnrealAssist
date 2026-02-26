// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "BlueprintCommandContext.h"

void FBlueprintCommandContext::AddReferencedObjects(FReferenceCollector& InReferenceCollector)
{
	InReferenceCollector.AddReferencedObject(Blueprint);
}

bool FBlueprintCommandContext::IsValid() const
{
	return UICommandInfo.IsValid() && ExecuteAction.IsBound() && ::IsValid(Blueprint) && !TargetModes.IsEmpty();
}
