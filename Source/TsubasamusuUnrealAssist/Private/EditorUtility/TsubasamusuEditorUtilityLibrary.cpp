// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "EditorUtility/TsubasamusuEditorUtilityLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Debug/TsubasamusuLogUtility.h"
#include "UObject/SavePackage.h"

#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION <= 2)
#include "Materials/MaterialInstanceDynamic.h"
#endif

UMaterialInstance* UTsubasamusuEditorUtilityLibrary::CreateMaterialInstanceAsset(const UMaterialInstanceDynamic* InMaterialInstanceDynamic, const FString& InAssetDirectory)
{
    if (!IsValid(InMaterialInstanceDynamic))
    {
        TUA_ERROR(TEXT("UMaterialInstanceDynamic is not valid."));

        return nullptr;
    }

    if (!InAssetDirectory.StartsWith(TEXT("/Game/")))
    {
        TUA_ERROR(TEXT("Asset directory does not start with \"/Game/.\""));

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
        TUA_ERROR(TEXT("Failed to create UPackage."));

        return nullptr;
    }

    UMaterialInstanceConstant* MaterialInstanceConstant = NewObject<UMaterialInstanceConstant>(Package, FName(*MaterialInstanceName), RF_Public | RF_Standalone);

    if (!IsValid(MaterialInstanceConstant))
    {
        TUA_ERROR(TEXT("Failed to create UMaterialInstanceConstant."));

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
        TUA_ERROR(TEXT("Failed to save package."));

        return nullptr;
    }

    return CastChecked<UMaterialInstance>(MaterialInstanceConstant);
}

bool UTsubasamusuEditorUtilityLibrary::SavePackage(UPackage* InPackage, UObject* InBaseAsset, const FString& InFileName)
{
    FSavePackageArgs SavePackageArgs;

    SavePackageArgs.TopLevelFlags = RF_Public | RF_Standalone;
    SavePackageArgs.Error = GError;
    SavePackageArgs.bForceByteSwapping = true;
    SavePackageArgs.SaveFlags = SAVE_NoError;

    return UPackage::SavePackage(InPackage, InBaseAsset, *InFileName, SavePackageArgs);
}
