// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "AccessSpecifierInitializer.h"
#include "Engine/Blueprint.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_FunctionEntry.h"
#include "Blueprint/BlueprintMemberUtility.h"
#include "Type/TsubasamusuUnrealAssistStructs.h"

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

void FAccessSpecifierInitializer::UnregisterBlueprint(UBlueprint* InBlueprint)
{
	if (IsValid(InBlueprint))
	{
		const FBlueprintMemberSet FoundBlueprintMemberSet = FindBlueprintMemberSet(InBlueprint);
	
		if (FoundBlueprintMemberSet.IsValid())
		{
			UnregisterBlueprintMemberSet(FoundBlueprintMemberSet);
		}
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
	
}

FBlueprintMemberSet FAccessSpecifierInitializer::CreateBlueprintMemberSet(UBlueprint* InBlueprint)
{
	if (IsValid(InBlueprint))
	{
		TArray<FName> VariableNames;
		TArray<FFunctionSet> FunctionSets;
		TArray<FCustomEventSet> CustomEventSets;
		
		// Variables
		{
			const TArray<FProperty*> Variables = FBlueprintMemberUtility::GetVariables(InBlueprint);
	
			for (const FProperty* Variable : Variables)
			{
				VariableNames.Add(Variable->GetFName());
			}
		}
		
		// Functions
		{
			TMap<UFunction*, UK2Node_FunctionEntry*> Functions = FBlueprintMemberUtility::GetFunctions(InBlueprint);
			
			for (const TPair<UFunction*, UK2Node_FunctionEntry*>& Function : Functions)
			{
				FFunctionSet FunctionSet(Function.Key->GetFName(), Function.Key, Function.Value);
				FunctionSets.Add(FunctionSet);
			}
		}

		// Custom Events
		{
			TMap<UFunction*, UK2Node_CustomEvent*> CustomEvents = FBlueprintMemberUtility::GetEvents(InBlueprint);
			
			for (const TPair<UFunction*, UK2Node_CustomEvent*>& CustomEvent : CustomEvents)
			{
				FCustomEventSet CustomEventSet(CustomEvent.Key->GetFName(), CustomEvent.Key, CustomEvent.Value);
				CustomEventSets.Add(CustomEventSet);
			}
		}

		return FBlueprintMemberSet(InBlueprint, VariableNames, FunctionSets, CustomEventSets);
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
