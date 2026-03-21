// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "TsubasamusuUnrealAssistEnums.h"
#include "TsubasamusuUnrealAssistMacros.h"
#include "TsubasamusuUnrealAssistStructs.generated.h"

class UK2Node_FunctionEntry;
#if CUSTOM_EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
class UK2Node_CustomEvent;
#endif

using FShouldAddMenu = TFunction<bool(const TArray<TWeakObjectPtr<UEdGraphNode>>& /*InSelectedNodes*/)>;
using FOnSelectedNodeMenuClicked = TFunction<void(const TArray<TWeakObjectPtr<UEdGraphNode>>& /*InSelectedNodes*/, const TWeakObjectPtr<UEdGraph> /*InGraph*/)>;
using FOnSelectedNodeSubMenuClicked = TFunction<void(const TArray<TWeakObjectPtr<UEdGraphNode>>& /*InSelectedNodes*/, const TWeakObjectPtr<UEdGraph> /*InGraph*/, const FText& /*InSubMenuLabelText*/)>;

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

struct FFunctionSet
{
public:
	FFunctionSet(const FName& InFunctionName, const TWeakObjectPtr<UFunction> InFunction, const TWeakObjectPtr<UK2Node_FunctionEntry> InFunctionEntryNode)
		: FunctionName(InFunctionName), Function(InFunction), FunctionEntryNode(InFunctionEntryNode){}
	
	FName FunctionName;
	TWeakObjectPtr<UFunction> Function;
	TWeakObjectPtr<UK2Node_FunctionEntry> FunctionEntryNode;

	bool IsValid() const
	{
		return !FunctionName.IsNone()
			&& Function.IsValid()
			&& FunctionEntryNode.IsValid();
	}

	bool operator==(const FFunctionSet& InFunctionSet) const
	{
		return FunctionName == InFunctionSet.FunctionName;
	}
};

#if CUSTOM_EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
struct FCustomEventSet
{
public:
	FCustomEventSet(const FName& InCustomEventName, const TWeakObjectPtr<UFunction> InFunction, const TWeakObjectPtr<UK2Node_CustomEvent> InCustomEventEntryNode)
		: CustomEventName(InCustomEventName), Function(InFunction), CustomEventEntryNode(InCustomEventEntryNode){}
	
	FName CustomEventName;
	TWeakObjectPtr<UFunction> Function;
	TWeakObjectPtr<UK2Node_CustomEvent> CustomEventEntryNode;

	bool IsValid() const
	{
		return !CustomEventName.IsNone()
			&& Function.IsValid()
			&& CustomEventEntryNode.IsValid();
	}

	bool operator==(const FCustomEventSet& InCustomEventSet) const
	{
		return CustomEventName == InCustomEventSet.CustomEventName;
	}
};
#endif

struct FBlueprintMemberSet
{
public:
	FBlueprintMemberSet() : Blueprint(nullptr){}
#if CUSTOM_EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
	FBlueprintMemberSet(const TWeakObjectPtr<UBlueprint> InBlueprint, const TArray<FName>& InVariableNames, const TArray<FFunctionSet>& InFunctionSets, const TArray<FCustomEventSet>& InCustomEventSets)
		: Blueprint(InBlueprint), VariableNames(InVariableNames), FunctionSets(InFunctionSets), CustomEventSets(InCustomEventSets){}
#else
	FBlueprintMemberSet(const TWeakObjectPtr<UBlueprint> InBlueprint, const TArray<FName>& InVariableNames, const TArray<FFunctionSet>& InFunctionSets)
		: Blueprint(InBlueprint), VariableNames(InVariableNames), FunctionSets(InFunctionSets){}
#endif
	
	TWeakObjectPtr<UBlueprint> Blueprint;
	TArray<FName> VariableNames;
	TArray<FFunctionSet> FunctionSets;
#if CUSTOM_EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
	TArray<FCustomEventSet> CustomEventSets;
#endif
	FDelegateHandle BlueprintChangedEventHandle;

	bool IsValid() const
	{
		return Blueprint.IsValid();
	}

	bool operator==(const FBlueprintMemberSet& InBlueprintMemberSet) const
	{
		return Blueprint == InBlueprintMemberSet.Blueprint
			&& VariableNames == InBlueprintMemberSet.VariableNames
#if CUSTOM_EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
			&& CustomEventSets == InBlueprintMemberSet.CustomEventSets
#endif
			&& FunctionSets == InBlueprintMemberSet.FunctionSets;
	}
	
	bool operator!=(const FBlueprintMemberSet& InBlueprintMemberSet) const
	{
		return !(*this == InBlueprintMemberSet);
	}
};

USTRUCT()
struct FConfigLlamaServerOption
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	FSoftClassPath SoftClassPath;
	
	UPROPERTY()
	FString Argument;
	
	bool operator==(const FConfigLlamaServerOption& InConfigLlamaServerOption) const
	{
		return SoftClassPath == InConfigLlamaServerOption.SoftClassPath
			&& Argument == InConfigLlamaServerOption.Argument;
	}
};

struct FLlamaServerSettings
{
public:
	FFilePath LlamaServerFilePath;
	TArray<FConfigLlamaServerOption> ConfigLlamaServerOptions;
	
	void Reset()
	{
		*this = FLlamaServerSettings();
	}
	
	bool operator==(const FLlamaServerSettings& InLlamaServerSettings) const
	{
		return InLlamaServerSettings.ConfigLlamaServerOptions == ConfigLlamaServerOptions
			&& LlamaServerFilePath.FilePath == InLlamaServerSettings.LlamaServerFilePath.FilePath;
	}
};

struct FSelectedNodeSubMenuContext
{
public:
	FOnSelectedNodeSubMenuClicked OnClicked;
	
	FText LabelText;
	FText ToolTipText;
};

struct FSelectedNodeMenuContext
{
public:
	FShouldAddMenu ShouldAddMenu;
	FOnSelectedNodeMenuClicked OnClicked;
	
	FText LabelText;
	FText ToolTipText;
	
	FSlateIcon MenuIcon;
	TArray<FSelectedNodeSubMenuContext> SubMenuContexts;
};
