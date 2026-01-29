// Copyright (c) 2026, tsubasamusu All rights reserved.

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
	static void GenerateComment(const FString& NodeDataListString, const TFunction<void(const bool bSucceeded, const FString& Message)>& OnGeneratedComment);

	static TArray<UEdGraphNode*> GetActiveNodes(const TArray<UEdGraphNode*>& InNodes);
	static int32 GetCharNum(const FString& InString, const TCHAR& InChar);
	
	static bool HasConnectedPins(const UEdGraphNode* InNode);
	
	static bool TryGetGptRequestString(const FString& NodeDataListString, FString& OutGptRequestString);
	static FString GetDesiredPrompt(const FString& NodeDataListString);

	static TArray<UEdGraphNode*> GetNodesUnderComment(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);
	static bool TryGetNodeDataListStringUnderComment(FString& OutNodeDataListString, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);
};
