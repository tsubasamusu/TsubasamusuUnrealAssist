// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "ContentBrowser/ZipAssetUtility.h"
#include "AssetRegistry/AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "FZipAssetUtility"

void FZipAssetUtility::AddZipAssetMenu(FToolMenuSection& InSection, const TArray<FName>& InSelectedAssetPackageNames)
{
	const TArray<FAssetData> DirtyAssetDataList = GetDirtyAssetDataList(InSelectedAssetPackageNames);
	bool bContainsDirtyAssets = !DirtyAssetDataList.IsEmpty();
	
	const FCanExecuteAction CanExecuteActionDelegate = FCanExecuteAction::CreateLambda([bContainsDirtyAssets]()
	{
		return !bContainsDirtyAssets;
	});
	
	const FName MenuName = TEXT("ZipAssetMenu");
	const FText MenuLabelText = LOCTEXT("ZipAssetMenuLabelText", "ZIP Asset(s)");
	const FText MenuToolTipText = LOCTEXT("ZipAssetMenuTooltip", "Compress the selected asset(s) into a ZIP file.");

	const FSlateIcon MenuIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "MainFrame.ZipUpProject");
	const FUIAction MenuAction = FUIAction(FExecuteAction::CreateStatic(&ExecuteZipAssetAction, InSelectedAssetPackageNames));
	
	InSection.AddMenuEntry(MenuName, MenuLabelText, MenuToolTipText, MenuIcon, MenuAction);
}

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

void FZipAssetUtility::ExecuteZipAssetAction(TArray<FName> InSelectedAssetPackageNames)
{
}

FString FZipAssetUtility::GetDesiredDefaultFileName(const TArray<FName>& InSelectedAssetPackageNames)
{
	if (InSelectedAssetPackageNames.Num() == 1)
	{
		return FPackageName::GetShortName(InSelectedAssetPackageNames[0]);
	}

	const FString LongPackagePath = FPackageName::GetLongPackagePath(InSelectedAssetPackageNames[0].ToString());

	int32 LastSlashIndex;
	LongPackagePath.FindLastChar(TEXT('/'), LastSlashIndex);
	
	return LongPackagePath.Mid(LastSlashIndex + 1);
}

#undef LOCTEXT_NAMESPACE
