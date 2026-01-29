// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "EditorUtility/AsyncActionReplaceReferences.h"
#include "AssetDeleteModel.h"
#include "TsubasamusuLogUtility.h"

UAsyncActionReplaceReferences* UAsyncActionReplaceReferences::AsyncReplaceReferences(const UObject* WorldContextObject, TSoftObjectPtr<UObject> From, TSoftObjectPtr<UObject> To)
{
    if (!IsValid(WorldContextObject))
    {
        FTsubasamusuLogUtility::LogError(TEXT("WorldContextObject is invalid."));
        
        return nullptr;
    }
    
    UAsyncActionReplaceReferences* Action = NewObject<UAsyncActionReplaceReferences>();

    Action->SourceAsset = From;
    Action->DestinationAsset = To;

    Action->RegisterWithGameInstance(WorldContextObject);

    return Action;
}

void UAsyncActionReplaceReferences::Activate()
{
    LoadedSourceAsset = SourceAsset.IsValid()? SourceAsset.Get(): SourceAsset.LoadSynchronous();
    LoadedDestinationAsset = DestinationAsset.IsValid()? DestinationAsset.Get() : DestinationAsset.LoadSynchronous();

    if (!IsValid(LoadedSourceAsset) || !IsValid(LoadedDestinationAsset))
    {
        FTsubasamusuLogUtility::LogError(TEXT("Failed to load assets."));
        
        OnCompleted(false);

        return;
    }
    
    constexpr float ScanSpan = 0.1f;

    const TArray<UObject*> SourceAssets = { LoadedSourceAsset };
    TSharedPtr<FAssetDeleteModel> AssetDeleteModel = MakeShared<FAssetDeleteModel>(SourceAssets);
    
    TSharedPtr<FAssetData> DestinationAssetData = MakeShared<FAssetData>(LoadedDestinationAsset);

    AssetDeleteModel->OnStateChanged().AddLambda([this, ScanSpan, AssetDeleteModel, DestinationAssetData](const FAssetDeleteModel::EState NewState)
    {
        if (NewState == FAssetDeleteModel::EState::Finished)
        {
            const bool bSucceeded = AssetDeleteModel->DoReplaceReferences(*DestinationAssetData);

            OnCompleted(bSucceeded);

            return;
        }
        
        AssetDeleteModel->Tick(ScanSpan);
    });

    AssetDeleteModel->Tick(ScanSpan);
}

void UAsyncActionReplaceReferences::OnCompleted(const bool bSucceeded)
{
	Completed.Broadcast(bSucceeded);

	SetReadyToDestroy();
}
