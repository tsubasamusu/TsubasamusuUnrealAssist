// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityLibrary.h"
#include "TsubasamusuEditorUtilityLibrary.generated.h"

UCLASS()
class UTsubasamusuEditorUtilityLibrary final : public UEditorUtilityLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TsubasamusuUnrealAssist|EditorUtility")
	static UMaterialInstance* CreateMaterialInstanceAsset(const UMaterialInstanceDynamic* InMaterialInstanceDynamic, const FString& InAssetDirectory);

private:
	static bool SavePackage(UPackage* InPackage, UObject* BaseAsset, const FString& FileName);
};
