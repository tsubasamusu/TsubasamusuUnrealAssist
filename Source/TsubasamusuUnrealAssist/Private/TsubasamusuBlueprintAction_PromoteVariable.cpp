// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "TsubasamusuBlueprintAction_PromoteVariable.h"
#include "BlueprintEditor.h"
#include "Kismet2/BlueprintEditorUtils.h"

UEdGraphNode* FTsubasamusuBlueprintAction_PromoteVariable::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2f& Location, bool bSelectNewNode)
{
	if(!ParentGraph || !FromPin)
	{
		return nullptr;
	}
	
	UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(ParentGraph);

	if(MyBlueprintEditor.IsValid() && Blueprint)
	{
		MyBlueprintEditor.Pin()->DoPromoteToVariable2f(Blueprint, FromPin, bToMemberVariable, Location);
	}
	
	return nullptr;
}
