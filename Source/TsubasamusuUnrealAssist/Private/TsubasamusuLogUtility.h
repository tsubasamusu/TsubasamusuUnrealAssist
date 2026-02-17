// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "Widgets/Notifications/SNotificationList.h"
#include "CoreMinimal.h"

#define LOCTEXT_NAMESPACE "TsubasamusuUnrealAssist"

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
	enum class EDialogButton : uint8
	{
		OK,
		Cancel,
		Close
	};
	
	static void DisplaySimpleNotification(const FText& InNotificationText, const SNotificationItem::ECompletionState CompletionState = SNotificationItem::ECompletionState::CS_None);
	static EAppReturnType::Type OpenWarningMessageDialog(const EAppMsgType::Type InMessageType, const FText& InMessage);
	
#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 6)
	static EDialogButton ShowCustomDialog(const FText& Title, const FText& Message, const FText& OkButtonText = LOCTEXT("OkButtonLabel", "OK"), const FText& CancelButtonText = LOCTEXT("CancelButtonLabel", "Cancel"), const TSharedPtr<SWidget> ContentWidget = nullptr, TAttribute<bool> OkButtonIsEnabled = true);
#else
	static EDialogButton ShowCustomDialog(const FText& Title, const FText& Message, const FText& OkButtonText = LOCTEXT("OkButtonLabel", "OK"), const FText& CancelButtonText = LOCTEXT("CancelButtonLabel", "Cancel"), const TSharedPtr<SWidget> ContentWidget = nullptr);
#endif
};

#undef LOCTEXT_NAMESPACE
