// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions/AssetTypeActions_Blueprint.h"

class FAssetTypeActions_TsubasamusuBlueprint : public FAssetTypeActions_Blueprint
{
public:
	
	//~ Begin IAssetTypeActions Interface
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	//~ End IAssetTypeActions Interface

private:
	
	static bool ShouldUseDataOnlyEditor(const UBlueprint* Blueprint);
};
