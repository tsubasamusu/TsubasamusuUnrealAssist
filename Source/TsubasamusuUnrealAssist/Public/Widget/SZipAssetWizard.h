// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "Widgets/SCompoundWidget.h"

class SZipAssetWizard : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_TwoParams(FOnZipAssetWizardCompleted, const TArray<FName>&, const FString&)

	SLATE_BEGIN_ARGS(SZipAssetWizard) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TArray<FName>& InInitialAssetPackageNames, const TArray<FName>& InAssetPackageNames, const FOnZipAssetWizardCompleted& InOnZipAssetWizardCompleted);
	static void OpenWizard(const TArray<FName>& InInitialAssetPackageNames, const TArray<FName>& AssetPackageNames, const FOnZipAssetWizardCompleted& InOnZipAssetWizardCompleted);
};
