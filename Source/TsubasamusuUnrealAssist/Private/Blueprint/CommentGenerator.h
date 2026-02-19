// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class UEdGraphNode_Comment;

class FCommentGenerator final
{
public:
	static void AddCommentGenerationMenu(FMenuBuilder& InMenuBuilder, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);

private:
	static void UpdateCommentByGpt(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);
	static void GenerateComment(const FString& InNodeDataListString, const TFunction<void(const bool bInSucceeded, const FString& InMessage)>& InGeneratedCommentFunction);

	static TArray<UEdGraphNode*> GetActiveNodes(const TArray<UEdGraphNode*>& InNodes);
	static int32 GetCharNum(const FString& InString, const TCHAR& InChar);
	
	static bool HasConnectedPins(const UEdGraphNode* InNode);
	
	static bool TryGetGptRequestString(const FString& InNodeDataListString, FString& OutGptRequestString);
	static FString GetDesiredPrompt(const FString& InNodeDataListString);

	static TArray<UEdGraphNode*> GetNodesUnderComment(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);
	static bool TryGetNodeDataListStringUnderComment(FString& OutNodeDataListString, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);
};
