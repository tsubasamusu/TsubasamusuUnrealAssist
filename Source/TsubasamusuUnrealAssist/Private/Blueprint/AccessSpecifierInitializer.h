// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Type/TsubasamusuUnrealAssistStructs.h"

class FAccessSpecifierInitializer final
{
public:
	void RegisterBlueprint(UBlueprint* InBlueprint);
	void UnregisterBlueprint(UBlueprint* InBlueprint);
	void UnregisterAllBlueprints();

private:
	void OnBlueprintChanged(UBlueprint* InBlueprint);
	static FBlueprintMemberInformation CreateBlueprintMemberInformation(UBlueprint* InBlueprint);
	void UnregisterBlueprint(const FBlueprintMemberInformation& InBlueprintMemberInformation);
	
	bool IsRegisteredBlueprint(UBlueprint* InBlueprint);
	FBlueprintMemberInformation FindBlueprintMemberInformation(UBlueprint* InBlueprint);
	
	TArray<FBlueprintMemberInformation> BlueprintMemberInformationArray;
};
