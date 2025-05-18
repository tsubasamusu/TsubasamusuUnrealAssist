// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityLibrary.h"
#include "TsubasamusuEditorUtility.generated.h"

UCLASS()
class TSUBASAMUSUUNREALASSIST_API UTsubasamusuEditorUtility : public UEditorUtilityLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TSUBASAMUSU|EditorUtility")
	static UMaterialInstance* CreateMaterialInstanceAsset(const UMaterialInstanceDynamic* InMaterialInstanceDynamic, const FString& InAssetDirectory);

	static bool SavePackage(UPackage* InPackage, UObject* BaseAsset, const FString& FileName);
};