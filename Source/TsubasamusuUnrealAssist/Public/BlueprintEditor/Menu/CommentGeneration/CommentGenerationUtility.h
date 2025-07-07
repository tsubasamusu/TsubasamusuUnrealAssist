// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

struct FNodeDataList;
struct FPinData;
class UEdGraphNode_Comment;

class TSUBASAMUSUUNREALASSIST_API FCommentGenerationUtility final
{
public:
	
	static void AddCommentGenerationMenu(FMenuBuilder& InMenuBuilder, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);

private:

	static void UpdateCommentByGpt(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);

	static FNodeDataList GetNodeDataList(const TArray<UEdGraphNode*>& InNodes);
	static TArray<FPinData> GetPinDataList(const UEdGraphNode* InNode);

	static TArray<FString> GetPinIds(const TArray<UEdGraphPin*>& InPins);
	static TArray<UEdGraphNode*> GetActiveNodes(const TArray<UEdGraphNode*>& InNodes);

	static bool HasConnectedPins(const UEdGraphNode* InNode);
	static bool IsCommentNode(const UEdGraphNode* InNode);
	static bool IsPinUsesDefaultValue(const UEdGraphPin* InPin);
	
	static int32 GetCharNum(const FString& InString, const TCHAR& InChar);
	static FString GetPinTypeAsString(const UEdGraphPin* InPin);
	
	static void GenerateComment(const FString& NodeDataListString, const TFunction<void(const bool bSucceeded, const FString& Message)>& OnGeneratedComment);

	static bool TryGetGptRequestString(const FString& NodeDataListString, FString& OutGptRequestString);
	static FString GetDesiredPrompt(const FString& NodeDataListString);

	static TArray<UEdGraphNode*> GetNodesUnderComment(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);
	static bool TryGetNodeDataListStringUnderComment(FString& OutNodeDataListString, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);
};
