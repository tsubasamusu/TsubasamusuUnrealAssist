// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "TsubasamusuLogUtility.h"
#include "Dialog/SCustomDialog.h"
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

FTsubasamusuLogUtility::EDialogButton FTsubasamusuLogUtility::ShowCustomDialog(const FText& Title, const FText& Message, const FText& OkButtonText, const FText& CancelButtonText, const TSharedPtr<SWidget> ContentWidget, TAttribute<bool> OkButtonIsEnabled)
{
	const TSharedRef<SCustomDialog> CustomDialog = SNew(SCustomDialog)
		.Title(Title)
		.Content()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(10)
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(Message)
				.AutoWrapText(true)
			]
			+ SVerticalBox::Slot()
			.FillHeight(0.8)
			[
				(ContentWidget.IsValid() ? ContentWidget.ToSharedRef() : SNullWidget::NullWidget)
			]
		]
		.Buttons(
		{
			SCustomDialog::FButton(OkButtonText)
				.SetPrimary(true)
				.SetIsEnabled(OkButtonIsEnabled),
			SCustomDialog::FButton(CancelButtonText)
		});

	const int32 PressedButtonIndex = CustomDialog->ShowModal();

	switch (PressedButtonIndex)
	{
		case -1:
			return EDialogButton::Close;
		case 0:
			return EDialogButton::OK;
		case 1:
			return EDialogButton::Cancel;
		default:
			checkNoEntry()
			return EDialogButton::Close;
	}
}

#undef LOCTEXT_NAMESPACE
