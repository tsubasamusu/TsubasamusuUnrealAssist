// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "EditorUtility/AsyncActionReplaceReferences.h"
#include "AssetDeleteModel.h"
#include "Message/TsubasamusuLogUtility.h"
#include "Type/TsubasamusuUnrealAssistMacros.h"

UAsyncActionReplaceReferences* UAsyncActionReplaceReferences::AsyncReplaceReferences(const UObject* WorldContextObject, TSoftObjectPtr<UObject> InSourceAsset, TSoftObjectPtr<UObject> InDestinationAsset)
{
    if (!IsValid(WorldContextObject))
    {
        TUA_ERROR(TEXT("WorldContextObject is invalid."));
        
        return nullptr;
    }
    
    UAsyncActionReplaceReferences* Action = NewObject<UAsyncActionReplaceReferences>();

    Action->SourceAsset = InSourceAsset;
    Action->DestinationAsset = InDestinationAsset;
    
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 1, 0)
    Action->RegisterWithGameInstance(WorldContextObject);
#else
    Action->RegisterWithGameInstance(const_cast<UObject*>(WorldContextObject));
#endif

    return Action;
}

void UAsyncActionReplaceReferences::Activate()
{
    LoadedSourceAsset = SourceAsset.IsValid()? SourceAsset.Get(): SourceAsset.LoadSynchronous();
    LoadedDestinationAsset = DestinationAsset.IsValid()? DestinationAsset.Get() : DestinationAsset.LoadSynchronous();

    if (!IsValid(LoadedSourceAsset) || !IsValid(LoadedDestinationAsset))
    {
        TUA_ERROR(TEXT("Failed to load assets."));
        
        OnCompleted(false);

        return;
    }
    
    constexpr float ScanSpan = 0.1f;

    const TArray<UObject*> SourceAssets = { LoadedSourceAsset };
    TSharedPtr<FAssetDeleteModel> AssetDeleteModel = MakeShared<FAssetDeleteModel>(SourceAssets);
    
    TSharedPtr<FAssetData> DestinationAssetData = MakeShared<FAssetData>(LoadedDestinationAsset);

    AssetDeleteModel->OnStateChanged().AddLambda([this, ScanSpan, AssetDeleteModel, DestinationAssetData](const FAssetDeleteModel::EState InNewState)
    {
        if (InNewState == FAssetDeleteModel::EState::Finished)
        {
            const bool bSucceeded = AssetDeleteModel->DoReplaceReferences(*DestinationAssetData);

            OnCompleted(bSucceeded);

            return;
        }
        
        AssetDeleteModel->Tick(ScanSpan);
    });

    AssetDeleteModel->Tick(ScanSpan);
}

void UAsyncActionReplaceReferences::OnCompleted(const bool bInSucceeded)
{
	Completed.Broadcast(bInSucceeded);

	SetReadyToDestroy();
}
