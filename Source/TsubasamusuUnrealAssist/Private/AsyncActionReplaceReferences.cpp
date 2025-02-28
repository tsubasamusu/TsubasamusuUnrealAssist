// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "AsyncActionReplaceReferences.h"
#include "TsubasamusuLogUtility.h"
#include "AssetDeleteModel.h"

UAsyncActionReplaceReferences* UAsyncActionReplaceReferences::AsyncReplaceReferences(const UObject* WorldContextObject, UObject* From, UObject* To)
{
    if (!IsValid(WorldContextObject))
    {
        FTsubasamusuLogUtility::LogError(TEXT("WorldContextObject is not valid."));

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
    if (!IsValid(SourceAsset))
    {
        FTsubasamusuLogUtility::LogError(TEXT("Source asset is not valid."));

        OnCompleted(false);

        return;
    }

    if (!IsValid(DestinationAsset))
    {
        FTsubasamusuLogUtility::LogError(TEXT("Destination asset is not valid."));

        OnCompleted(false);

        return;
    }

    const float ScanSpanSeconds = 0.1f;

    const TArray<UObject*> SourceAssets = { SourceAsset };

    TSharedPtr<FAssetDeleteModel> AssetDeleteModel = MakeShared<FAssetDeleteModel>(SourceAssets);

    TSharedPtr<FAssetData> DestinationAssetData = MakeShared<FAssetData>(DestinationAsset);

    AssetDeleteModel->OnStateChanged().AddLambda([this, ScanSpanSeconds, AssetDeleteModel, DestinationAssetData](FAssetDeleteModel::EState NewState)
        {
            if (NewState != FAssetDeleteModel::EState::Finished)
            {
                AssetDeleteModel->Tick(ScanSpanSeconds);

                return;
            }

            const bool bSucceeded = AssetDeleteModel->DoReplaceReferences(*DestinationAssetData);

            OnCompleted(bSucceeded);
        });

    AssetDeleteModel->Tick(ScanSpanSeconds);
}

void UAsyncActionReplaceReferences::OnCompleted(const bool bSucceeded)
{
	Completed.Broadcast(bSucceeded);

	SetReadyToDestroy();
}