// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "ContentBrowser/ZipAssetUtility.h"
#include "AssetRegistry/AssetRegistryModule.h"

TArray<FAssetData> FZipAssetUtility::GetDirtyAssetDataList(const TArray<FName>& InAssetPackageNames)
{
	TArray<FAssetData> AssetDataList;
	AssetDataList.Reserve(InAssetPackageNames.Num());

	const IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();

	for (const FName& AssetPackageName : InAssetPackageNames)
	{
		TArray<FAssetData> _AssetDataList;
		check(AssetRegistry.GetAssetsByPackageName(AssetPackageName, _AssetDataList));

		TArray<FAssetData> ValidAssetDataList = _AssetDataList.FilterByPredicate([](const FAssetData& InAssetData)
		 {
			 const UObject* Asset = InAssetData.GetAsset();
			 return IsValid(Asset) && !Asset->IsA<UBlueprintGeneratedClass>();
		 });
	
		AssetDataList.Append(ValidAssetDataList);
	}
	
	TArray<FAssetData> DirtyAssetDataList;

	for (const FAssetData& AssetData : AssetDataList)
	{
		const UPackage* Package = AssetData.GetPackage();

		if (IsValid(Package) && Package->IsDirty())
		{
			DirtyAssetDataList.Add(AssetData);
		}
	}
	
	return DirtyAssetDataList;
}
