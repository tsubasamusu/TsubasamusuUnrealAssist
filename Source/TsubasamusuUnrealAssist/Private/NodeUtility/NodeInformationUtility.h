// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NodeInformationUtility.generated.h"

USTRUCT()
struct FPinData
{
	GENERATED_BODY()

public:
	UPROPERTY();
	FString PinName;

	UPROPERTY()
	FString PinDirection;

	UPROPERTY()
	FString PinType;
    
	UPROPERTY()
	FString PinId;

	UPROPERTY()
	FString DefaultValue;

	UPROPERTY()
	bool bThisPinUsesDefaultValue = false;

	UPROPERTY()
	TArray<FString> ConnectedPinIds;
};

USTRUCT()
struct FNodeData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString NodeName;

	UPROPERTY()
	FString Comment;

	UPROPERTY()
	bool bIsCommentNode = false;

	UPROPERTY()
	TArray<FPinData> PinDataList;
};

USTRUCT()
struct FNodeDataList
{
	GENERATED_BODY()

public:
	FNodeDataList() : NodeDataList(TArray<FNodeData>()){}
	explicit FNodeDataList(const TArray<FNodeData>& InNodeDataList) : NodeDataList(InNodeDataList){}
    
	UPROPERTY()
	TArray<FNodeData> NodeDataList;
};

class FNodeInformationUtility final
{
public:
	static bool TryGetNodeDataListString(FString& OutNodeDataListString, const TArray<UEdGraphNode*>& InNodes);
	static bool TryGetNodeDataListString(FString& OutNodeDataListString, const TArray<TWeakObjectPtr<UEdGraphNode>>& InWeakNodes);
	static bool TryGetNodeDataListToonString(FString& OutNodeDataListString, const TArray<TWeakObjectPtr<UEdGraphNode>>& InWeakNodes);
	
	static FNodeDataList GetNodeDataList(const TArray<UEdGraphNode*>& InNodes);
	static TArray<FPinData> GetPinDataList(const UEdGraphNode* InNode);

	static TArray<FString> GetPinIds(const TArray<UEdGraphPin*>& InPins);
	static FString GetPinDirectionAsString(const UEdGraphPin* InPin);
	static FString GetPinTypeAsString(const UEdGraphPin* InPin);

	static bool IsPinUsesDefaultValue(const UEdGraphPin* InPin);
	static bool IsCommentNode(const UEdGraphNode* InNode);
};
