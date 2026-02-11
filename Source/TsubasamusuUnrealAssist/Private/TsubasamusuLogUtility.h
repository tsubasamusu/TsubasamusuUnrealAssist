// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "Widgets/Notifications/SNotificationList.h"
#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTsubasamusuUnrealAssist, Log, All);

#define TUA_LOG(Format, ...) \
{ \
	UE_LOG(LogTsubasamusuUnrealAssist, Log, Format, ##__VA_ARGS__); \
}

#define TUA_WARNING(Format, ...) \
{ \
	UE_LOG(LogTsubasamusuUnrealAssist, Warning, Format, ##__VA_ARGS__); \
}

#define TUA_ERROR(Format, ...) \
{ \
	UE_LOG(LogTsubasamusuUnrealAssist, Error, Format, ##__VA_ARGS__); \
}

class FTsubasamusuLogUtility final
{
public:
	static void DisplaySimpleNotification(const FText& InNotificationText, const SNotificationItem::ECompletionState CompletionState);
};
