// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(TsubasamusuUnrealAssistLog, Log, All);

class TSUBASAMUSUUNREALASSIST_API FTsubasamusuLogUtility
{
public:
	static void Log(const FString& InMessage);
	static void LogError(const FString& InMessage);
	static void LogWarning(const FString& InMessage);
};
