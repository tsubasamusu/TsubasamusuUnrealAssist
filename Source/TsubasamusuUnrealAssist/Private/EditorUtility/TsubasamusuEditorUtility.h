// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityLibrary.h"
#include "TsubasamusuEditorUtility.generated.h"

UCLASS()
class UTsubasamusuEditorUtility final : public UEditorUtilityLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TsubasamusuUnrealAssist|EditorUtility")
	static UMaterialInstance* CreateMaterialInstanceAsset(const UMaterialInstanceDynamic* InMaterialInstanceDynamic, const FString& InAssetDirectory);

	static bool SavePackage(UPackage* InPackage, UObject* BaseAsset, const FString& FileName);
};
