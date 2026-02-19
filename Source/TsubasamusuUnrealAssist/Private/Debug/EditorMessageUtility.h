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
	
#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 6)
	static TsubasamusuUnrealAssist::EDialogButton ShowCustomDialog(const FText& InTitle, const FText& InMessage, const FText& InOkButtonText = LOCTEXT("OkButtonLabel", "OK"), const FText& InCancelButtonText = LOCTEXT("CancelButtonLabel", "Cancel"), const TSharedPtr<SWidget> InContentWidget = nullptr, TAttribute<bool> InOkButtonIsEnabled = true);
#else
	static TsubasamusuUnrealAssist::EDialogButton ShowCustomDialog(const FText& InTitle, const FText& InMessage, const FText& InOkButtonText = LOCTEXT("OkButtonLabel", "OK"), const FText& InCancelButtonText = LOCTEXT("CancelButtonLabel", "Cancel"), const TSharedPtr<SWidget> InContentWidget = nullptr);
#endif
};

#undef LOCTEXT_NAMESPACE
