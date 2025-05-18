// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "TsubasamusuLogUtility.h"

DEFINE_LOG_CATEGORY(TsubasamusuUnrealAssistLog);

void FTsubasamusuLogUtility::Log(const FString& InMessage)
{
	UE_LOG(TsubasamusuUnrealAssistLog, Log, TEXT("%s"), *InMessage);
}

void FTsubasamusuLogUtility::LogError(const FString& InMessage)
{
	UE_LOG(TsubasamusuUnrealAssistLog, Error, TEXT("%s"), *InMessage);
}

void FTsubasamusuLogUtility::LogWarning(const FString& InMessage)
{
	UE_LOG(TsubasamusuUnrealAssistLog, Warning, TEXT("%s"), *InMessage);
}
