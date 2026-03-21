// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AccessSpecifierInitializer.generated.h"

struct FBlueprintMemberSet;

UCLASS()
class UAccessSpecifierInitializer final : public UEditorSubsystem
{
	GENERATED_BODY()
	
public:
	//~ Begin USubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& InSubsystemCollectionBase) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface
	
private:
	void OnBlueprintChanged(UBlueprint* InBlueprint);
	
	void RegisterBlueprint(UBlueprint* InBlueprint);
	void UnregisterAllBlueprints();

	bool IsRegisteredBlueprint(UBlueprint* InBlueprint);
	FBlueprintMemberSet FindBlueprintMemberSet(UBlueprint* InBlueprint);
	static FBlueprintMemberSet CreateBlueprintMemberSet(UBlueprint* InBlueprint);
	
	TArray<FBlueprintMemberSet> BlueprintMemberSets;
	FDelegateHandle AssetEditorOpenedHandle;
};
