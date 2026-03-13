// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "TsubasamusuUnrealAssistEnums.h"
#include "TsubasamusuUnrealAssistStructs.generated.h"

class UK2Node_CustomEvent;
class UK2Node_FunctionEntry;

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

	bool IsEmpty() const
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
		const ETsubasamusuAccessSpecifier InCurrentAccessSpecifier = ETsubasamusuAccessSpecifier::None,
		const ETsubasamusuAccessSpecifier InOptimalAccessSpecifier = ETsubasamusuAccessSpecifier::None)
		: MemberName(InMemberName),
		  CurrentAccessSpecifier(InCurrentAccessSpecifier),
		  OptimalAccessSpecifier(InOptimalAccessSpecifier) {}
	
	FName MemberName;
	ETsubasamusuAccessSpecifier CurrentAccessSpecifier;
	ETsubasamusuAccessSpecifier OptimalAccessSpecifier;
};

struct FFunctionInformation
{
public:
	FFunctionInformation(const FName& InFunctionName, const TWeakObjectPtr<UFunction> InFunction, const TWeakObjectPtr<UK2Node_FunctionEntry> InFunctionEntryNode)
		: FunctionName(InFunctionName), Function(InFunction), FunctionEntryNode(InFunctionEntryNode){}
	
	FName FunctionName;
	TWeakObjectPtr<UFunction> Function;
	TWeakObjectPtr<UK2Node_FunctionEntry> FunctionEntryNode;
};

struct FCustomEventInformation
{
public:
	FCustomEventInformation(const FName& InCustomEventName, const TWeakObjectPtr<UFunction> InFunction, const TWeakObjectPtr<UK2Node_CustomEvent> InCustomEventEntryNode)
		: CustomEventName(InCustomEventName), Function(InFunction), CustomEventEntryNode(InCustomEventEntryNode){}
	
	FName CustomEventName;
	TWeakObjectPtr<UFunction> Function;
	TWeakObjectPtr<UK2Node_CustomEvent> CustomEventEntryNode;
};

struct FBlueprintMemberInformation
{
public:
	FBlueprintMemberInformation() : Blueprint(nullptr) {}
	FBlueprintMemberInformation(const TWeakObjectPtr<UBlueprint> InBlueprint, const TArray<FName>& InVariableNames, const TArray<FFunctionInformation>& InFunctions, const TArray<FCustomEventInformation>& InCustomEvents)
		: Blueprint(InBlueprint), VariableNames(InVariableNames), Functions(InFunctions), CustomEvents(InCustomEvents){}
	
	TWeakObjectPtr<UBlueprint> Blueprint;
	TArray<FName> VariableNames;
	TArray<FFunctionInformation> Functions;
	TArray<FCustomEventInformation> CustomEvents;
	FDelegateHandle BlueprintChangedEventHandle;

	bool IsValid() const
	{
		return Blueprint.IsValid();
	}

	bool operator==(const FBlueprintMemberInformation& InAnotherBlueprintMemberInformation) const
	{
		return Blueprint == InAnotherBlueprintMemberInformation.Blueprint;
	}
};
