// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

struct FNodeData;
struct FPinData;
class UEdGraphNode_Comment;

class TSUBASAMUSUUNREALASSIST_API FCommentGenerationUtility final
{
public:
	
	static void AddCommentGenerationMenu(FMenuBuilder& InMenuBuilder, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);

private:

	static void GenerateComment(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode);

	static TArray<FNodeData> GetNodeDataList(const TArray<UEdGraphNode*>& InNodes);
	static TArray<FPinData> GetPinDataList(const UEdGraphNode* InNode);

	static TArray<FString> GetPinIds(const TArray<UEdGraphPin*>& InPins);
	static TArray<UEdGraphNode*> GetActiveNodes(const TArray<UEdGraphNode*>& InNodes);

	static bool HasConnectedPins(const UEdGraphNode* InNode);
	static bool IsCommentNode(const UEdGraphNode* InNode);
	static bool IsPinUsesDefaultValue(const UEdGraphPin* InPin);
	
	static int32 GetCharNum(const FString& InString, const TCHAR& InChar);
	static FString GetPinTypeAsString(const UEdGraphPin* InPin);
};
