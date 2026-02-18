// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "TsubasamusuUnrealAssistEnums.h"
#include "TsubasamusuUnrealAssistStructs.generated.h"

USTRUCT()
struct FGptMessage
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString role;

	UPROPERTY()
	FString content;
};

USTRUCT()
struct FGptRequest
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString model;

	UPROPERTY()
	TArray<FGptMessage> messages;
};

USTRUCT()
struct FGptResponseChoice
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FGptMessage message;
};

USTRUCT()
struct FGptResponse
{
	GENERATED_BODY()

public:
	FORCEINLINE bool IsEmpty() const
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

	FORCEINLINE FString GetGptMessage()const
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
struct FGptError
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
struct FGptErrorResponse
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FGptError error;

	FORCEINLINE bool IsEmpty() const
	{
		return error.message.IsEmpty() && error.type.IsEmpty() && error.code.IsEmpty();
	}
};

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

struct FLocalizedCulturesFlyweight
{
	TArray<FCultureRef> LocalizedCultures;

	FLocalizedCulturesFlyweight()
	{
		const TArray<FString> LocalizedCultureNames = FTextLocalizationManager::Get().GetLocalizedCultureNames(ELocalizationLoadFlags::Editor);
		LocalizedCultures = FInternationalization::Get().GetAvailableCultures(LocalizedCultureNames, false);
	}
};

struct FAccessSpecifierOptimizationRow
{
public:
	explicit FAccessSpecifierOptimizationRow(
		const FName& InMemberName = NAME_None,
		const TsubasamusuUnrealAssist::EBlueprintMember InMemberType = TsubasamusuUnrealAssist::EBlueprintMember::None,
		const TsubasamusuUnrealAssist::EAccessSpecifier InCurrentAccessSpecifier = TsubasamusuUnrealAssist::EAccessSpecifier::None,
		const TsubasamusuUnrealAssist::EAccessSpecifier InRecommendedAccessSpecifier = TsubasamusuUnrealAssist::EAccessSpecifier::None,
		const bool bInSelected = true)
		: MemberName(InMemberName),
		  MemberType(InMemberType),
		  CurrentAccessSpecifier(InCurrentAccessSpecifier),
		  RecommendedAccessSpecifier(InRecommendedAccessSpecifier),
		  bSelected(bInSelected) {}
	
	FName MemberName;
	TsubasamusuUnrealAssist::EBlueprintMember MemberType;
	TsubasamusuUnrealAssist::EAccessSpecifier CurrentAccessSpecifier;
	TsubasamusuUnrealAssist::EAccessSpecifier RecommendedAccessSpecifier;
	bool bSelected;
	TSharedPtr<SCheckBox> CheckBox;
};
