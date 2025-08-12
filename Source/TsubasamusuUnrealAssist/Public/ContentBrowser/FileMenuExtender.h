// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class TSUBASAMUSUUNREALASSIST_API FFileMenuExtender final
{
public:
	static void RegisterFileMenu();

private:
	static void AddFileMenu();

	static void GetSelectedAssetPackageNames(const UToolMenu* InMenu, TArray<FString>& OutSelectedAssetPackageNames);
	static void GetSelectedAssetPackageNames(const UToolMenu* InMenu, TArray<FName>& OutSelectedAssetPackageNames);
};
