// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NodeData.generated.h"

USTRUCT()
struct TSUBASAMUSUUNREALASSIST_API FPinData
{
    GENERATED_BODY()

public:
    
    UPROPERTY();
    FString PinName;

    UPROPERTY()
    FString PinType;

    UPROPERTY()
    FString PinCategory;
    
    UPROPERTY()
    FString PinId;

    UPROPERTY()
    FString DefaultValue;

    UPROPERTY()
    bool bThisPinUsesDefaultValue;

    UPROPERTY()
    TArray<FString> ConnectedPinIds;
};

USTRUCT()
struct TSUBASAMUSUUNREALASSIST_API FNodeData
{
    GENERATED_BODY()

public:
    
    UPROPERTY()
    FString NodeName;

    UPROPERTY()
    FString Comment;

    UPROPERTY()
    bool bIsCommentNode;

    UPROPERTY()
    TArray<FPinData> PinDataList;
};

USTRUCT()
struct TSUBASAMUSUUNREALASSIST_API FNodeDataList
{
    GENERATED_BODY()

public:
    
    FNodeDataList() : NodeDataList(TArray<FNodeData>())
    {
        
    }

    explicit FNodeDataList(const TArray<FNodeData>& InNodeDataList) : NodeDataList(InNodeDataList)
    {
        
    }
    
    UPROPERTY()
    TArray<FNodeData> NodeDataList;
};