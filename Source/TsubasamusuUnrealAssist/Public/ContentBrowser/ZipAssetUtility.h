// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class TSUBASAMUSUUNREALASSIST_API FZipAssetUtility final
{
public:
	static void AddZipAssetMenu(FToolMenuSection& InSection, const TArray<FName>& InSelectedAssetPackageNames);
	
private:
	static TArray<FAssetData> GetDirtyAssetDataList(const TArray<FName>& InAssetPackageNames);
	static void ExecuteZipAssetAction(TArray<FName> InSelectedAssetPackageNames);
	static FString GetDesiredDefaultFileName(const TArray<FName>& InSelectedAssetPackageNames);
};
