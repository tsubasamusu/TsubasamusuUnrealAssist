// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "Types/TsubasamusuUnrealAssistEnums.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "CoreMinimal.h"

#define LOCTEXT_NAMESPACE "TsubasamusuUnrealAssist"

class FEditorMessageUtility final
{
public:
	static void DisplaySimpleNotification(const FText& InNotificationText, const SNotificationItem::ECompletionState CompletionState = SNotificationItem::ECompletionState::CS_None);
	
	static EAppReturnType::Type OpenWarningMessageDialog(const EAppMsgType::Type InMessageType, const FText& InMessage);
	
#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 6)
	static TsubasamusuUnrealAssist::EDialogButton ShowCustomDialog(const FText& Title, const FText& Message, const FText& OkButtonText = LOCTEXT("OkButtonLabel", "OK"), const FText& CancelButtonText = LOCTEXT("CancelButtonLabel", "Cancel"), const TSharedPtr<SWidget> ContentWidget = nullptr, TAttribute<bool> OkButtonIsEnabled = true);
#else
	static EDialogButton ShowCustomDialog(const FText& Title, const FText& Message, const FText& OkButtonText = LOCTEXT("OkButtonLabel", "OK"), const FText& CancelButtonText = LOCTEXT("CancelButtonLabel", "Cancel"), const TSharedPtr<SWidget> ContentWidget = nullptr);
#endif
};

#undef LOCTEXT_NAMESPACE
