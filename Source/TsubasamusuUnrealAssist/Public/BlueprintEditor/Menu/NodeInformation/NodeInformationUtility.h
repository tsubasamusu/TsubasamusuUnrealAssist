// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

struct FNodeDataList;
struct FPinData;

class TSUBASAMUSUUNREALASSIST_API FNodeInformationUtility final
{
public:
	static bool TryGetNodeDataListString(FString& OutNodeDataListString, const TArray<UEdGraphNode*>& InNodes);
	static FNodeDataList GetNodeDataList(const TArray<UEdGraphNode*>& InNodes);
	static TArray<FPinData> GetPinDataList(const UEdGraphNode* InNode);

	static TArray<FString> GetPinIds(const TArray<UEdGraphPin*>& InPins);
	static FString GetPinDirectionAsString(const UEdGraphPin* InPin);
	static FString GetPinTypeAsString(const UEdGraphPin* InPin);

	static bool IsPinUsesDefaultValue(const UEdGraphPin* InPin);
	static bool IsCommentNode(const UEdGraphNode* InNode);
};
