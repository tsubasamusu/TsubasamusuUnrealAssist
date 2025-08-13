// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "Widget/SZipAssetWizard.h"
#include "Interfaces/IMainFrameModule.h"

#define LOCTEXT_NAMESPACE "SZipAssetWizard"

void SZipAssetWizard::Construct(const FArguments& InArgs, const TArray<FName>& InInitialAssetPackageNames, const TArray<FName>& InAssetPackageNames, const FOnZipAssetWizardCompleted& InOnZipAssetWizardCompleted)
{
	
}

void SZipAssetWizard::OpenWizard(const TArray<FName>& InInitialAssetPackageNames, const TArray<FName>& AssetPackageNames, const FOnZipAssetWizardCompleted& InOnZipAssetWizardCompleted)
{
	const TSharedRef<SWindow> ReportWindow = SNew(SWindow)
		.Title(LOCTEXT("Window_Title", "Tsubasamusu Unreal Assist | Zip the selected asset(s)"))
		.ClientSize(FVector2D(960, 700))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		[
			SNew(SZipAssetWizard, InInitialAssetPackageNames, AssetPackageNames, InOnZipAssetWizardCompleted)
		];

	const IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	const TSharedPtr<SWindow> ParentWindow = MainFrameModule.GetParentWindow();
	
	if (ParentWindow.IsValid())
	{
		FSlateApplication::Get().AddWindowAsNativeChild(ReportWindow, MainFrameModule.GetParentWindow().ToSharedRef());
	}
	else
	{
		FSlateApplication::Get().AddWindow(ReportWindow);
	}
}

#undef LOCTEXT_NAMESPACE
