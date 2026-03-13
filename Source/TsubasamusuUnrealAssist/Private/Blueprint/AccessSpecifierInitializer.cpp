// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "AccessSpecifierInitializer.h"
#include "Engine/Blueprint.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_FunctionEntry.h"
#include "Blueprint/BlueprintMemberUtility.h"
#include "Type/TsubasamusuUnrealAssistStructs.h"

void FAccessSpecifierInitializer::RegisterBlueprint(UBlueprint* InBlueprint)
{
	if (IsValid(InBlueprint) && !IsRegisteredBlueprint(InBlueprint))
	{
		FBlueprintMemberInformation BlueprintMemberInformation = CreateBlueprintMemberInformation(InBlueprint);
		BlueprintMemberInformation.BlueprintChangedEventHandle = InBlueprint->OnChanged().AddStatic(&FAccessSpecifierInitializer::OnBlueprintChanged);
		BlueprintMemberInformationArray.Add(BlueprintMemberInformation);
	}
}

void FAccessSpecifierInitializer::UnregisterBlueprint(UBlueprint* InBlueprint)
{
	const FBlueprintMemberInformation FoundBlueprintMemberInformation = FindBlueprintMemberInformation(InBlueprint);
	
	if (IsValid(InBlueprint) && FoundBlueprintMemberInformation.IsValid())
	{
		UnregisterBlueprint(FoundBlueprintMemberInformation);
	}
}

void FAccessSpecifierInitializer::UnregisterAllBlueprints()
{
	for (const FBlueprintMemberInformation& BlueprintMemberInformation : BlueprintMemberInformationArray)
	{
		UnregisterBlueprint(BlueprintMemberInformation);
	}
	
	BlueprintMemberInformationArray.Empty();
}

void FAccessSpecifierInitializer::OnBlueprintChanged(UBlueprint* InBlueprint)
{
	
}

FBlueprintMemberInformation FAccessSpecifierInitializer::CreateBlueprintMemberInformation(UBlueprint* InBlueprint)
{
	if (IsValid(InBlueprint))
	{
		TArray<FName> VariableNames;
		TArray<FFunctionInformation> FunctionInformationArray;
		TArray<FCustomEventInformation> CustomEventInformationArray;
		
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
				FFunctionInformation FunctionInformation(Function.Key->GetFName(), Function.Key, Function.Value);
				FunctionInformationArray.Add(FunctionInformation);
			}
		}

		// Custom Events
		{
			TMap<UFunction*, UK2Node_CustomEvent*> CustomEvents = FBlueprintMemberUtility::GetEvents(InBlueprint);
			
			for (const TPair<UFunction*, UK2Node_CustomEvent*>& CustomEvent : CustomEvents)
			{
				FCustomEventInformation CustomEventInformation(CustomEvent.Key->GetFName(), CustomEvent.Key, CustomEvent.Value);
				CustomEventInformationArray.Add(CustomEventInformation);
			}
		}

		return FBlueprintMemberInformation(InBlueprint, VariableNames, FunctionInformationArray, CustomEventInformationArray);
	}
	
	return FBlueprintMemberInformation();
}

void FAccessSpecifierInitializer::UnregisterBlueprint(const FBlueprintMemberInformation& InBlueprintMemberInformation)
{
	if (InBlueprintMemberInformation.IsValid())
	{
		InBlueprintMemberInformation.Blueprint->OnChanged().Remove(InBlueprintMemberInformation.BlueprintChangedEventHandle);
		BlueprintMemberInformationArray.Remove(InBlueprintMemberInformation);
	}
}

bool FAccessSpecifierInitializer::IsRegisteredBlueprint(UBlueprint* InBlueprint)
{
	const FBlueprintMemberInformation FoundBlueprintMemberInformation = FindBlueprintMemberInformation(InBlueprint);
	return FoundBlueprintMemberInformation.IsValid();
}

FBlueprintMemberInformation FAccessSpecifierInitializer::FindBlueprintMemberInformation(UBlueprint* InBlueprint)
{
	FBlueprintMemberInformation* FoundBlueprintMemberInformation = BlueprintMemberInformationArray.FindByPredicate([InBlueprint](const FBlueprintMemberInformation& InBlueprintMemberInformation)
	{
		return InBlueprintMemberInformation.Blueprint.IsValid() && InBlueprintMemberInformation.Blueprint.Get() == InBlueprint;
	});
	
	return FoundBlueprintMemberInformation ? *FoundBlueprintMemberInformation : FBlueprintMemberInformation();
}
