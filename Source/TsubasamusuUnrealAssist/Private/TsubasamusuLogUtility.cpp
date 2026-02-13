// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "TsubasamusuLogUtility.h"
#include "Framework/Notifications/NotificationManager.h"

DEFINE_LOG_CATEGORY(LogTsubasamusuUnrealAssist);

#define LOCTEXT_NAMESPACE "TsubasamusuUnrealAssist"

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

EAppReturnType::Type FTsubasamusuLogUtility::OpenWarningMessageDialog(const EAppMsgType::Type InMessageType, const FText& InMessage)
{
	const FText Title = LOCTEXT("WarningMessageDialogTitle", "Tsubasamusu Unreal Assist Warning");
                
#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3)
	return FMessageDialog::Open(InMessageType, InMessage, Title);
#else
	return FMessageDialog::Open(InMessageType, InMessage, &Title);
#endif
}

#undef LOCTEXT_NAMESPACE
