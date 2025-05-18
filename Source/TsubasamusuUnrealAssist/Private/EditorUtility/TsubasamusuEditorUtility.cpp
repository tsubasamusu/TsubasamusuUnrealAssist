// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "EditorUtility/TsubasamusuEditorUtility.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Materials/MaterialInstanceConstant.h"
#include "TsubasamusuLogUtility.h"
#include "UObject/SavePackage.h"

UMaterialInstance* UTsubasamusuEditorUtility::CreateMaterialInstanceAsset(const UMaterialInstanceDynamic* InMaterialInstanceDynamic, const FString& InAssetDirectory)
{
    if (!IsValid(InMaterialInstanceDynamic))
    {
        FTsubasamusuLogUtility::LogError(TEXT("UMaterialInstanceDynamic is not valid."));

        return nullptr;
    }

    if (!InAssetDirectory.StartsWith(TEXT("/Game/")))
    {
        FTsubasamusuLogUtility::LogError(TEXT("Asset directory does not start with \"/Game/\"."));

        return nullptr;
    }

    const FString MaterialInstanceName = InMaterialInstanceDynamic->GetName();
    FString AssetPath = InAssetDirectory / MaterialInstanceName;

    if (AssetPath.Contains(TEXT("//")))
    {
        AssetPath.ReplaceInline(TEXT("//"), TEXT("/"));
    }

    UPackage* Package = CreatePackage(*AssetPath);

    if (!IsValid(Package))
    {
        FTsubasamusuLogUtility::LogError(TEXT("Failed to create UPackage."));

        return nullptr;
    }

    UMaterialInstanceConstant* MaterialInstanceConstant = NewObject<UMaterialInstanceConstant>(Package, FName(*MaterialInstanceName), RF_Public | RF_Standalone);

    if (!IsValid(MaterialInstanceConstant))
    {
        FTsubasamusuLogUtility::LogError(TEXT("Failed to create UMaterialInstanceConstant."));

        return nullptr;
    }

    MaterialInstanceConstant->SetParentEditorOnly(InMaterialInstanceDynamic->Parent);
    MaterialInstanceConstant->ScalarParameterValues = InMaterialInstanceDynamic->ScalarParameterValues;
    MaterialInstanceConstant->VectorParameterValues = InMaterialInstanceDynamic->VectorParameterValues;
    MaterialInstanceConstant->TextureParameterValues = InMaterialInstanceDynamic->TextureParameterValues;

    FAssetRegistryModule::AssetCreated(MaterialInstanceConstant);

    MaterialInstanceConstant->MarkPackageDirty();

    const FString FileName = FPackageName::LongPackageNameToFilename(AssetPath, FPackageName::GetAssetPackageExtension());

    if (!SavePackage(Package, MaterialInstanceConstant, FileName))
    {
        FTsubasamusuLogUtility::LogError(TEXT("Failed to save package."));

        return nullptr;
    }

    return CastChecked<UMaterialInstance>(MaterialInstanceConstant);
}

bool UTsubasamusuEditorUtility::SavePackage(UPackage* InPackage, UObject* BaseAsset, const FString& FileName)
{
    FSavePackageArgs SavePackageArgs;

    SavePackageArgs.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
    SavePackageArgs.Error = GError;
    SavePackageArgs.bForceByteSwapping = true;
    SavePackageArgs.SaveFlags = SAVE_NoError;

    return UPackage::SavePackage(InPackage, BaseAsset, *FileName, SavePackageArgs);
}
