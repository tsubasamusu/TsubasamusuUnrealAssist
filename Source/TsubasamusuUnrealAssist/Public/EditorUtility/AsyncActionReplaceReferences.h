// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityLibrary.h"
#include "AsyncActionReplaceReferences.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCompletedReplaceReferences, const bool, bSucceeded);

UCLASS()
class TSUBASAMUSUUNREALASSIST_API UAsyncActionReplaceReferences : public UEditorUtilityBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable)
	FOnCompletedReplaceReferences Completed;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "TSUBASAMUSU|EditorUtility")
	static UAsyncActionReplaceReferences* AsyncReplaceReferences(const UObject* WorldContextObject, TSoftObjectPtr<UObject> From, TSoftObjectPtr<UObject> To);

	virtual void Activate() override;

private:
	
	UPROPERTY()
	TSoftObjectPtr<UObject> SourceAsset;

	UPROPERTY()
	TSoftObjectPtr<UObject> DestinationAsset;

	UPROPERTY()
	TObjectPtr<UObject> LoadedSourceAsset;

	UPROPERTY()
	TObjectPtr<UObject> LoadedDestinationAsset;

	void OnCompleted(const bool bSucceeded);
};
