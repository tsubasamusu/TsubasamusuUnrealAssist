// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class FBlueprintEditor;
class FBlueprintCommandContext;

class FBlueprintEditorUtility final
{
public:
	static void RegisterCommandInBlueprintEditMenu(const FBlueprintCommandContext& InBlueprintCommandContext);
	static TSharedPtr<FBlueprintEditor> GetBlueprintEditor(const UBlueprint* InBlueprint);

private:
	static FString ConvertToTitleCaseString(const FString& InSourceString);
};
