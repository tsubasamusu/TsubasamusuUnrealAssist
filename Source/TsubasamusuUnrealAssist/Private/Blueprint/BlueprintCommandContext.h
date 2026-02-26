// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

class FBlueprintCommandContext final : public FGCObject
{
public:
	FBlueprintCommandContext(const TSharedPtr<const FUICommandInfo>& InUICommandInfo, const FExecuteAction& InExecuteAction, const TObjectPtr<UBlueprint> InBlueprint, const TArray<FName>& InTargetModes)
		: UICommandInfo(InUICommandInfo), ExecuteAction(InExecuteAction), Blueprint(InBlueprint), TargetModes(InTargetModes){}
	
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
	TArray<FName> TargetModes;
};
