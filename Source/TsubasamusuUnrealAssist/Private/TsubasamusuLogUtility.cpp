// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "TsubasamusuLogUtility.h"
#include "Framework/Notifications/NotificationManager.h"

DEFINE_LOG_CATEGORY(LogTsubasamusuUnrealAssist);

void FTsubasamusuLogUtility::DisplaySimpleNotification(const FText& InNotificationText, const SNotificationItem::ECompletionState CompletionState)
{
	FNotificationInfo NotificationInfo(InNotificationText);
	NotificationInfo.ExpireDuration = 3.f;
	NotificationInfo.bUseLargeFont = false;
	
	TSharedPtr<SNotificationItem> NotificationItem = FSlateNotificationManager::Get().AddNotification(NotificationInfo);
	
	if (NotificationItem.IsValid())
	{
		NotificationItem->SetCompletionState(CompletionState);
	}
}
