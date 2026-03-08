// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "EditorMessageUtility.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Type/TsubasamusuUnrealAssistMacros.h"

#if UE_VERSION_OLDER_THAN(5, 1, 0)
#include "Dialogs/CustomDialog.h"
#else
#include "Dialog/SCustomDialog.h"
#endif

#define LOCTEXT_NAMESPACE "FEditorMessageUtility"

void FEditorMessageUtility::DisplaySimpleNotification(const FText& InNotificationText, const SNotificationItem::ECompletionState InCompletionState)
{
	FNotificationInfo NotificationInfo(InNotificationText);
	NotificationInfo.ExpireDuration = 3.f;
	NotificationInfo.bUseLargeFont = false;
	
	TSharedPtr<SNotificationItem> NotificationItem = FSlateNotificationManager::Get().AddNotification(NotificationInfo);
	
	if (NotificationItem.IsValid())
	{
		NotificationItem->SetCompletionState(InCompletionState);
	}
}

EAppReturnType::Type FEditorMessageUtility::OpenWarningMessageDialog(const EAppMsgType::Type InMessageType, const FText& InMessage)
{
	const FText Title = LOCTEXT("WarningMessageDialogTitle", "Tsubasamusu Unreal Assist Warning");
                
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 3, 0)
	return FMessageDialog::Open(InMessageType, InMessage, Title);
#else
	return FMessageDialog::Open(InMessageType, InMessage, &Title);
#endif
}

TsubasamusuUnrealAssist::EDialogButton FEditorMessageUtility::ShowCustomDialog(const FText& InTitle, const FText& InMessage, const FText& InOkButtonText, const FText& InCancelButtonText, const TSharedPtr<SWidget> InContentWidget, TAttribute<bool> InOkButtonIsEnabled)
{
	const TSharedRef<SCustomDialog> CustomDialog = SNew(SCustomDialog)
		.Title(InTitle)
#if UE_VERSION_OLDER_THAN(5, 1, 0)
		.DialogContent(
#else
		.Content()
		[
#endif
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(10)
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(InMessage)
				.AutoWrapText(true)
			]
			+ SVerticalBox::Slot()
			.FillHeight(0.8)
			[
				(InContentWidget.IsValid() ? InContentWidget.ToSharedRef() : SNullWidget::NullWidget)
			]
#if UE_VERSION_OLDER_THAN(5, 1, 0)
		)
#else
		]
#endif
		.Buttons(
		{
			SCustomDialog::FButton(InOkButtonText)
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 6, 0)
				.SetIsEnabled(InOkButtonIsEnabled)
#endif
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 1, 0)
				.SetPrimary(true)
#endif
			,
			SCustomDialog::FButton(InCancelButtonText)
		});

	const int32 PressedButtonIndex = CustomDialog->ShowModal();

	switch (PressedButtonIndex)
	{
		case -1:
			return TsubasamusuUnrealAssist::EDialogButton::Close;
		case 0:
			return TsubasamusuUnrealAssist::EDialogButton::OK;
		case 1:
			return TsubasamusuUnrealAssist::EDialogButton::Cancel;
		default:
			checkNoEntry()
			return TsubasamusuUnrealAssist::EDialogButton::Close;
	}
}

#undef LOCTEXT_NAMESPACE
