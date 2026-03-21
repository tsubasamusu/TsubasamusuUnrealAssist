// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystemBase.h"
#include "AccessSpecifierInitializer.generated.h"

struct FBlueprintMemberSet;

UCLASS()
class UAccessSpecifierInitializer final : public UEditorSubsystemBase
{
	GENERATED_BODY()
	
public:
	//~ Begin USubsystem Interface
	virtual void Deinitialize() override;
	//~ End USubsystem Interface
	
protected:
	//~ Begin UEditorSubsystemBase Interface
	virtual void OnAssetEditorOpened(UObject* InOpenedAsset) override;
	//~ End UEditorSubsystemBase Interface
	
private:
	void OnBlueprintChanged(UBlueprint* InBlueprint);
	
	void RegisterBlueprint(UBlueprint* InBlueprint);
	void UnregisterAllBlueprints();

	bool IsRegisteredBlueprint(UBlueprint* InBlueprint);
	FBlueprintMemberSet FindBlueprintMemberSet(UBlueprint* InBlueprint);
	static FBlueprintMemberSet CreateBlueprintMemberSet(UBlueprint* InBlueprint);
	
	TArray<FBlueprintMemberSet> BlueprintMemberSets;
};
