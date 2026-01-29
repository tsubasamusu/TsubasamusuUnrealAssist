// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GptRequest.h"
#include "GptResponse.generated.h"

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