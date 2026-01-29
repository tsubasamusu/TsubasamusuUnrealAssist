// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommentGenerationUtility.generated.h"

struct FNodeDataList;
struct FPinData;
class UEdGraphNode_Comment;

USTRUCT()
struct TSUBASAMUSUUNREALASSIST_API FGptMessage
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString role;

	UPROPERTY()
	FString content;
};

USTRUCT()
struct TSUBASAMUSUUNREALASSIST_API FGptRequest
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString model;

	UPROPERTY()
	TArray<FGptMessage> messages;
};

USTRUCT()
struct TSUBASAMUSUUNREALASSIST_API FGptResponseChoice
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FGptMessage message;
};

USTRUCT()
struct TSUBASAMUSUUNREALASSIST_API FGptResponse
{
	GENERATED_BODY()

public:
	bool IsEmpty() const
	{
		if (choices.Num() == 0)
		{
			return true;
		}

		if (choices[0].message.content.IsEmpty())
		{
			return true;
		}

		return false;
	}

	FString GetGptMessage()const
	{
		if (IsEmpty())
		{
			return FString();
		}

		return choices[0].message.content;
	}

private:
	UPROPERTY()
	TArray<FGptResponseChoice> choices;
};

USTRUCT()
struct TSUBASAMUSUUNREALASSIST_API FGptError
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString message;

	UPROPERTY()
	FString type;

	UPROPERTY()
	FString code;
};

USTRUCT()
struct TSUBASAMUSUUNREALASSIST_API FGptErrorResponse
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FGptError error;

	bool IsEmpty() const
	{
		return error.message.IsEmpty() && error.type.IsEmpty() && error.code.IsEmpty();
	}
};

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
