// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "AccessSpecifierInitializer.h"
#include "Engine/Blueprint.h"
#include "K2Node_FunctionEntry.h"
#include "Blueprint/BlueprintMemberUtility.h"
#include "Setting/EditorSettingsUtility.h"
#include "Setting/TsubasamusuUnrealAssistSettings.h"
#include "Type/TsubasamusuUnrealAssistStructs.h"

#if CUSTOM_EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
#include "K2Node_CustomEvent.h"
#endif

template<typename ArrayType>
static TArray<ArrayType> FindElementsOnlyInSecondArray(const TArray<ArrayType>& FirstArray, const TArray<ArrayType>& SecondArray)
{
	TArray<ArrayType> ElementsOnlyInSecondArray;

	for (const ArrayType& SecondArrayElement : SecondArray)
	{
		if (!FirstArray.Contains(SecondArrayElement))
		{
			ElementsOnlyInSecondArray.Add(SecondArrayElement);
		}
	}

	return ElementsOnlyInSecondArray;
}

void FAccessSpecifierInitializer::RegisterBlueprint(UBlueprint* InBlueprint)
{
	if (IsValid(InBlueprint) && !IsRegisteredBlueprint(InBlueprint))
	{
		FBlueprintMemberSet BlueprintMemberSet = CreateBlueprintMemberSet(InBlueprint);
		BlueprintMemberSet.BlueprintChangedEventHandle = InBlueprint->OnChanged().AddRaw(this, &FAccessSpecifierInitializer::OnBlueprintChanged);
		BlueprintMemberSets.Add(BlueprintMemberSet);
	}
}

void FAccessSpecifierInitializer::UnregisterAllBlueprints()
{
	for (const FBlueprintMemberSet& BlueprintMemberSet : BlueprintMemberSets)
	{
		UnregisterBlueprintMemberSet(BlueprintMemberSet);
	}
	
	BlueprintMemberSets.Empty();
}

void FAccessSpecifierInitializer::OnBlueprintChanged(UBlueprint* InBlueprint)
{
	const FBlueprintMemberSet PreviousBlueprintMemberSet = FindBlueprintMemberSet(InBlueprint);
	const FBlueprintMemberSet CurrentBlueprintMemberSet = CreateBlueprintMemberSet(InBlueprint);
	
	if (PreviousBlueprintMemberSet != CurrentBlueprintMemberSet)
	{
		const int32 Index = BlueprintMemberSets.Find(PreviousBlueprintMemberSet);
		BlueprintMemberSets[Index] = CurrentBlueprintMemberSet;
		
		const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
		
		if (TsubasamusuUnrealAssistSettings->bOverrideVariableDefaultAccessSpecifier && PreviousBlueprintMemberSet.VariableNames.Num() < CurrentBlueprintMemberSet.VariableNames.Num())
		{
			const TArray<FName> AddedVariableNames = FindElementsOnlyInSecondArray(PreviousBlueprintMemberSet.VariableNames, CurrentBlueprintMemberSet.VariableNames);
			check(AddedVariableNames.Num() == 1);
		
			FBlueprintMemberUtility::SetVariableAccessSpecifier(TsubasamusuUnrealAssistSettings->VariableDefaultAccessSpecifier, AddedVariableNames[0], InBlueprint);
		}
		else if (TsubasamusuUnrealAssistSettings->bOverrideFunctionDefaultAccessSpecifier && PreviousBlueprintMemberSet.FunctionSets.Num() < CurrentBlueprintMemberSet.FunctionSets.Num())
		{
			const TArray<FFunctionSet> AddedFunctionSets = FindElementsOnlyInSecondArray(PreviousBlueprintMemberSet.FunctionSets, CurrentBlueprintMemberSet.FunctionSets);
			check(AddedFunctionSets.Num() == 1);
		
			const FFunctionSet AddedFunctionSet = AddedFunctionSets[0];
			
			if (AddedFunctionSet.IsValid())
			{
				FBlueprintMemberUtility::SetFunctionAccessSpecifier(TsubasamusuUnrealAssistSettings->FunctionDefaultAccessSpecifier, AddedFunctionSet.Function.Get(), AddedFunctionSet.FunctionEntryNode.Get(), InBlueprint);
			}
		}
#if CUSTOM_EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
		else if (TsubasamusuUnrealAssistSettings->bOverrideCustomEventDefaultAccessSpecifier && PreviousBlueprintMemberSet.CustomEventSets.Num() < CurrentBlueprintMemberSet.CustomEventSets.Num())
		{
			const TArray<FCustomEventSet> AddedCustomEventSets = FindElementsOnlyInSecondArray(PreviousBlueprintMemberSet.CustomEventSets, CurrentBlueprintMemberSet.CustomEventSets);
			check(AddedCustomEventSets.Num() == 1);
		
			const FCustomEventSet AddedCustomEventSet = AddedCustomEventSets[0];
			
			if (AddedCustomEventSet.IsValid())
			{
				FBlueprintMemberUtility::SetCustomEventAccessSpecifier(TsubasamusuUnrealAssistSettings->CustomEventDefaultAccessSpecifier, AddedCustomEventSet.Function.Get(), AddedCustomEventSet.CustomEventEntryNode.Get(), InBlueprint);
			}
		}
#endif
	}
}

FBlueprintMemberSet FAccessSpecifierInitializer::CreateBlueprintMemberSet(UBlueprint* InBlueprint)
{
	if (IsValid(InBlueprint))
	{
		TArray<FName> VariableNames;
		{
			const TArray<FProperty*> Variables = FBlueprintMemberUtility::GetVariables(InBlueprint);
	
			for (const FProperty* Variable : Variables)
			{
				VariableNames.Add(Variable->GetFName());
			}
		}
		
		TArray<FFunctionSet> FunctionSets;
		{
			TMap<UFunction*, UK2Node_FunctionEntry*> Functions = FBlueprintMemberUtility::GetFunctions(InBlueprint);
			
			for (const TPair<UFunction*, UK2Node_FunctionEntry*>& Function : Functions)
			{
				FFunctionSet FunctionSet(Function.Key->GetFName(), Function.Key, Function.Value);
				FunctionSets.Add(FunctionSet);
			}
		}

#if CUSTOM_EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
		TArray<FCustomEventSet> CustomEventSets;
		{
			TMap<UFunction*, UK2Node_CustomEvent*> CustomEvents = FBlueprintMemberUtility::GetEvents(InBlueprint);
			
			for (const TPair<UFunction*, UK2Node_CustomEvent*>& CustomEvent : CustomEvents)
			{
				FCustomEventSet CustomEventSet(CustomEvent.Key->GetFName(), CustomEvent.Key, CustomEvent.Value);
				CustomEventSets.Add(CustomEventSet);
			}
		}
		
		return FBlueprintMemberSet(InBlueprint, VariableNames, FunctionSets, CustomEventSets);
#else
		return FBlueprintMemberSet(InBlueprint, VariableNames, FunctionSets);
#endif
	}
	
	return FBlueprintMemberSet();
}

void FAccessSpecifierInitializer::UnregisterBlueprintMemberSet(const FBlueprintMemberSet& InBlueprintMemberSet)
{
	if (InBlueprintMemberSet.IsValid())
	{
		InBlueprintMemberSet.Blueprint->OnChanged().Remove(InBlueprintMemberSet.BlueprintChangedEventHandle);
		BlueprintMemberSets.Remove(InBlueprintMemberSet);
	}
}

bool FAccessSpecifierInitializer::IsRegisteredBlueprint(UBlueprint* InBlueprint)
{
	const FBlueprintMemberSet FoundBlueprintMemberSet = FindBlueprintMemberSet(InBlueprint);
	return FoundBlueprintMemberSet.IsValid();
}

FBlueprintMemberSet FAccessSpecifierInitializer::FindBlueprintMemberSet(UBlueprint* InBlueprint)
{
	if (IsValid(InBlueprint))
	{
		FBlueprintMemberSet* FoundBlueprintMemberSet = BlueprintMemberSets.FindByPredicate([InBlueprint](const FBlueprintMemberSet& InBlueprintMemberSet)
		{
			return InBlueprintMemberSet.IsValid() && InBlueprintMemberSet.Blueprint.Get() == InBlueprint;
		});
		
		if (FoundBlueprintMemberSet)
		{
			return *FoundBlueprintMemberSet;
		}
	}
	
	return FBlueprintMemberSet();
}
