// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "BlueprintEditorModes.h"

class FBlueprintCommandContext final : public FGCObject
{
public:
	FBlueprintCommandContext(const TSharedPtr<const FUICommandInfo>& InUICommandInfo, const FExecuteAction& InExecuteAction, const TObjectPtr<UBlueprint> InBlueprint, const TArray<FCanExecuteAction>& InAdditionalConditionsToExecuteAction = TArray<FCanExecuteAction>(), const TArray<FName>& InTargetModes = { FBlueprintEditorApplicationModes::StandardBlueprintEditorMode })
		: UICommandInfo(InUICommandInfo), ExecuteAction(InExecuteAction), Blueprint(InBlueprint), AdditionalConditionsToExecuteAction(InAdditionalConditionsToExecuteAction), TargetModes(InTargetModes){}
	
	//~ Begin FGCObject Interface
	virtual void AddReferencedObjects(FReferenceCollector& InReferenceCollector) override;
	virtual FString GetReferencerName() const override
	{
		return TEXT("FBlueprintCommandContext");
	}
	//~ End FGCObject Interface
	
	bool IsValid() const;
	
	TSharedPtr<const FUICommandInfo> UICommandInfo;
	FExecuteAction ExecuteAction;
	TObjectPtr<UBlueprint> Blueprint;
	TArray<FCanExecuteAction> AdditionalConditionsToExecuteAction;
	TArray<FName> TargetModes;
};
