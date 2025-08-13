// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "ContentBrowser/ZipAssetUtility.h"
#include "DesktopPlatformModule.h"
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
	bool bOpenedFileDialog = false;
	TArray<FString> SelectedFileNames;
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

	const void* ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
	const FString FileDialogTitle = LOCTEXT("ZipAsset", "ZIP Asset(s) into...").ToString();
	const FString DefaultFilePath = FPaths::ProjectDir();
	const FString DefaultFileName = GetDesiredDefaultFileName(InSelectedAssetPackageNames);
	
	if (DesktopPlatform)
	{
		bOpenedFileDialog = DesktopPlatform->SaveFileDialog(ParentWindowHandle, FileDialogTitle, DefaultFilePath, DefaultFileName, TEXT("Zip file|*.zip"), EFileDialogFlags::None, SelectedFileNames);
	}
}

bool FZipAssetUtility::TryGetDependencies(const TArray<FName>& InAssetPackageNames, TArray<FName>& OutDependencies, FText& OutErrorText, const bool bInShouldValidatePackages)
{
	if (InAssetPackageNames.IsEmpty())
	{
		OutErrorText = LOCTEXT("AssetPackageNames_Empty", "Provided asset package names array is empty.");
		return false;
	}

	if (bInShouldValidatePackages && !AssetsAreValid(InAssetPackageNames, OutErrorText))
	{
		return false;
	}

	TArray<FString> AssetPackageNamesString;
	Algo::Transform(InAssetPackageNames, AssetPackageNamesString, [](const FName& InAssetPackageName)
	{
		return InAssetPackageName.ToString();
	});
	
	for (const FName& AssetPackageName : InAssetPackageNames)
	{
		OutDependencies.Add(AssetPackageName);
		RecursiveGetDependencies(AssetPackageName, OutDependencies);
	}

	OutDependencies.Sort(FNameLexicalLess());
	return true;
}

void FZipAssetUtility::RecursiveGetDependencies(const FName& InAssetPackageName, TArray<FName>& OutDependencies)
{
	TArray<FName> Dependencies;
	TArray<FAssetData> AssetDataList;
	
	if (!TryGetAssetData(InAssetPackageName.ToString(), AssetDataList, Dependencies))
	{
		return;
	}

	for (const FName& Dependency : Dependencies)
	{
		if (IsValidDependency(Dependency.ToString()))
		{
			if (!OutDependencies.Contains(Dependency))
			{
				OutDependencies.Add(Dependency);
				RecursiveGetDependencies(Dependency, OutDependencies);
			}
		}
	}
}

bool FZipAssetUtility::IsValidDependency(const FString& InDependency)
{
	TArray<FName> IgnoreDirectoryNames =
	{
		TEXT("/Engine"),
		TEXT("/Script")
	};
	
	for (const FName& IgnoreDirectoryName : IgnoreDirectoryNames)
	{
		if (InDependency.StartsWith(IgnoreDirectoryName.ToString()))
		{
			return false;
		}
	}

	return true;
}

bool FZipAssetUtility::TryGetAssetData(const FString& InAssetPackageName, TArray<FAssetData>& OutAssetDataList, TArray<FName>& OutDependencies)
{
	FString AssetFileName = InAssetPackageName;

	if (!FPackageName::DoesPackageExist(InAssetPackageName, &AssetFileName))
	{
		if (FPackageName::IsValidLongPackageName(InAssetPackageName, false))
		{
			const UPackage* Package = FindPackage(nullptr, *InAssetPackageName);
			const FString PackageExtension = Package && Package->ContainsMap() ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension();
			AssetFileName = FPackageName::LongPackageNameToFilename(InAssetPackageName, PackageExtension);
		}
	}

	const FString AbsoluteAssetFileName = FPaths::ConvertRelativePathToFull(AssetFileName);
	const EPackageExtension PackageExtension = FPackagePath::ParseExtension(AbsoluteAssetFileName);

	if (PackageExtension == EPackageExtension::Unspecified || PackageExtension == EPackageExtension::Custom)
	{
		return false;
	}

	IAssetRegistry::FLoadPackageRegistryData LoadPackageRegistryData(true);

	const IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
	AssetRegistry.LoadPackageRegistryData(AbsoluteAssetFileName, LoadPackageRegistryData);

	OutAssetDataList = MoveTemp(LoadPackageRegistryData.Data);

	for (const FName& Dependency : LoadPackageRegistryData.DataDependencies)
	{
		if (OutDependencies.Contains(Dependency))
		{
			continue;
		}
		
		if (FPackageName::IsValidLongPackageName(Dependency.ToString()))
		{
			OutDependencies.Add(Dependency);
		}
	}

	OutDependencies.Add(FName(*InAssetPackageName));
	return !OutAssetDataList.IsEmpty();
}

bool FZipAssetUtility::AssetsAreValid(const TArray<FName>& InAssetPackageNames, FText& OutErrorText)
{
	for (const FName& AssetPackageName : InAssetPackageNames)
	{
		FString AssetFileName;
		FString AssetPackageNameString = AssetPackageName.ToString();
		
		if (!FPackageName::DoesPackageExist(AssetPackageNameString, &AssetFileName))
		{
			OutErrorText = FText::Format(LOCTEXT("ValidateAssets_Asset_Invalid", "{0} does not exist on disk."), FText::FromString(AssetPackageNameString));
			return false;
		}
	}

	return true;
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
