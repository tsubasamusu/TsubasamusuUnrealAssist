// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "TsubasamusuBlueprintAction_PromoteVariable.generated.h"

#define LOCTEXT_NAMESPACE "TsubasamusuBlueprintAction_PromoteVariable"

USTRUCT()
struct FTsubasamusuBlueprintAction_PromoteVariable : public FEdGraphSchemaAction
{
	GENERATED_BODY()
	
	explicit FTsubasamusuBlueprintAction_PromoteVariable(): bToMemberVariable(false)
	{
		
	}

	explicit FTsubasamusuBlueprintAction_PromoteVariable(const bool bInToMemberVariable) : FEdGraphSchemaAction(FText(), bInToMemberVariable? LOCTEXT("PromoteToVariable", "Promote to variable") : LOCTEXT("PromoteToLocalVariable", "Promote to local variable"), bInToMemberVariable ? LOCTEXT("PromoteToVariable", "Promote to variable") : LOCTEXT("PromoteToLocalVariable", "Promote to local variable"), 1), bToMemberVariable(bInToMemberVariable)
	{
		
	}

	//~ Begin FEdGraphSchemaAction Interface
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2f& Location, bool bSelectNewNode = true) override;
	//~ End FEdGraphSchemaAction Interface

	TWeakPtr<class FBlueprintEditor> MyBlueprintEditor;
	bool bToMemberVariable;
};

#undef LOCTEXT_NAMESPACE
