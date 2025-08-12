// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class TSUBASAMUSUUNREALASSIST_API FZipAssetUtility final
{
private:
	static TArray<FAssetData> GetDirtyAssetDataList(const TArray<FName>& InAssetPackageNames);
};
