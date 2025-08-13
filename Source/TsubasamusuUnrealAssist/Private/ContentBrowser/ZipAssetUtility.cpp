// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "ContentBrowser/ZipAssetUtility.h"
#include "DesktopPlatformModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widget/SZipAssetWizard.h"
#include "Widgets/Notifications/SNotificationList.h"

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
	FScopedSlowTask ScopedSlowTask(InSelectedAssetPackageNames.Num(), LOCTEXT("MigratePackages_GatheringDependencies", "Gathering Dependencies..."));
	ScopedSlowTask.MakeDialog();
	ScopedSlowTask.EnterProgressFrame();
	
	TArray<FName> AssetPackageNamesToMove;
	FText ErrorText;

	if (!TryGetDependencies(InSelectedAssetPackageNames, AssetPackageNamesToMove, ErrorText))
	{
		OnFoundInvalidPackages(InSelectedAssetPackageNames, LOCTEXT("Zip_Warning_Notify_Heading", "Some files are invalid (not existing on disk)"));
		return;
	}
	
	const TArray<FName> FilteredAssetPackageNames = AssetPackageNamesToMove.FilterByPredicate([](const FName& InAssetPackageName)
	{
		return FPackageName::DoesPackageExist(InAssetPackageName.ToString());
	});
	
	if (FilteredAssetPackageNames.Num() == 0)
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("Zip_NoFilesToSave", "No files were found to zip"));
		return;
	}
	
	SZipAssetWizard::OpenWizard(InSelectedAssetPackageNames, FilteredAssetPackageNames, SZipAssetWizard::FOnZipAssetWizardCompleted::CreateStatic(&OnZipAssetWizardCompleted));
}

void FZipAssetUtility::OnZipAssetWizardCompleted(const TArray<FName>& InAssetPackageNames, const FString& InFilePath)
{
	// bool bOpenedFileDialog = false;
	// TArray<FString> SelectedFileNames;
	// IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	//
	// const void* ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
	// const FString FileDialogTitle = LOCTEXT("ZipAsset", "ZIP Asset(s) into...").ToString();
	// const FString DefaultFilePath = FPaths::ProjectDir();
	// const FString DefaultFileName = GetDesiredDefaultFileName(InSelectedAssetPackageNames);
	//
	// if (DesktopPlatform)
	// {
	// 	bOpenedFileDialog = DesktopPlatform->SaveFileDialog(ParentWindowHandle, FileDialogTitle, DefaultFilePath, DefaultFileName, TEXT("Zip file|*.zip"), EFileDialogFlags::None, SelectedFileNames);
	// }
	//
	// if (bOpenedFileDialog)
	// {
	// 	for (const FString& SelectedFileName : SelectedFileNames)
	// 	{
	// 		FString FinalFileName = FPaths::ConvertRelativePathToFull(SelectedFileName);
	// 		FString ProjectPath = FPaths::IsProjectFilePathSet() ? FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) : FPaths::RootDir() / FApp::GetProjectName();
	//
	// 		FString CommandLine = FString::Printf(TEXT("ZipProjectUp -nocompileeditor -project=\"%s\" -install=\"%s\""), *ProjectPath, *FinalFileName);
	//
	// 		IUATHelperModule::Get().CreateUatTask( CommandLine, GetTargetPlatformManager()->GetRunningTargetPlatform()->DisplayName(), LOCTEXT("ZipTaskName", "Zipping Up Project"),
	// 		LOCTEXT("ZipTaskShortName", "Zip Project Task"), FAppStyle::GetBrush(TEXT("MainFrame.CookContent")), nullptr, IUATHelperModule::UatTaskResultCallack(), FPaths::GetPath(FinalFileName));
	// 	}
	// }
}

void FZipAssetUtility::OnFoundInvalidPackages(const TArray<FName>& InAssetPackageNames, const FText& InHeadingText, const FSlateBrush* InBrush)
{
	TArray<FString> ErrorMessages;
	for (const FName& AssetPackageName : InAssetPackageNames)
	{
		if (!FPackageName::DoesPackageExist(AssetPackageName.ToString()))
		{
			FText ErrorMessage = FText::Format(LOCTEXT("Zip_InvalidFile", "{0} does not exist on disk"), FText::FromName(AssetPackageName));
			ErrorMessages.Add(ErrorMessage.ToString());
		}
	}
	
	if (ErrorMessages.IsEmpty())
	{
		return;
	}
	
	const FString ErrorMessagesString = TEXT("- ") + FString::Join(ErrorMessages, TEXT("\n- "));
	const FText NotificationText = FText::Format(LOCTEXT("Zip_Error_Notify_Format", "{0}:\n\n{1}"), InHeadingText, FText::FromString(ErrorMessagesString));
	
	FNotificationInfo NotifyInfo(NotificationText);
	NotifyInfo.WidthOverride = 480.f;
	NotifyInfo.ExpireDuration = 5.0f;
	NotifyInfo.FadeInDuration = 0.2f;
	NotifyInfo.FadeOutDuration = 1.0f;
	NotifyInfo.Image = InBrush ? InBrush : FAppStyle::GetBrush(TEXT("AssetEditor.CompileStatus.Overlay.Warning"));
	
	FSlateNotificationManager::Get().AddNotification(NotifyInfo);
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
