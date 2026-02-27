// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "Type/TsubasamusuUnrealAssistEnums.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "CoreMinimal.h"

#define LOCTEXT_NAMESPACE "FEditorMessageUtility"

class FEditorMessageUtility final
{
public:
	static void DisplaySimpleNotification(const FText& InNotificationText, const SNotificationItem::ECompletionState InCompletionState = SNotificationItem::ECompletionState::CS_None);
	
	static EAppReturnType::Type OpenWarningMessageDialog(const EAppMsgType::Type InMessageType, const FText& InMessage);
	static TsubasamusuUnrealAssist::EDialogButton ShowCustomDialog(const FText& InTitle, const FText& InMessage, const FText& InOkButtonText = LOCTEXT("OkButtonLabel", "OK"), const FText& InCancelButtonText = LOCTEXT("CancelButtonLabel", "Cancel"), const TSharedPtr<SWidget> InContentWidget = nullptr, TAttribute<bool> InOkButtonIsEnabled = true);
};

#undef LOCTEXT_NAMESPACE
