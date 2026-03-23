// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class UEdGraphNode_Comment;
struct FSelectedNodeMenuContext;

class FCommentGenerator final
{
public:
	static FSelectedNodeMenuContext CreateSelectedNodeMenuContext();

private:
	static void GenerateComment(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);
	
	static TArray<UEdGraphNode*> GetActiveNodes(const TArray<UEdGraphNode*>& InNodes);
	static bool HasConnectedPins(const UEdGraphNode* InNode);
	
	static FString GetDesiredPrompt(const FString& InNodeDataListString);

	static TArray<UEdGraphNode*> GetNodesUnderComment(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);
	static bool TryGetNodeDataListStringUnderComment(FString& OutNodeDataListString, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);
};
