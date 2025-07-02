// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "AssetTypeActions_TsubasamusuBlueprint.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

void FAssetTypeActions_TsubasamusuBlueprint::OpenAssetEditor( const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor )
{
	FAssetTypeActions_Blueprint::OpenAssetEditor(InObjects, EditWithinLevelEditor);
}

#undef LOCTEXT_NAMESPACE
