// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "FMessageDialogUtility.h"

#define LOCTEXT_NAMESPACE "TsubasamusuUnrealAssist"

EAppReturnType::Type FMessageDialogUtility::OpenWarningMessageDialog(const EAppMsgType::Type InMessageType, const FText& InMessage)
{
	const FText Title = LOCTEXT("WarningMessageDialogTitle", "Tsubasamusu Unreal Assist Warning");
                
#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3)
	return FMessageDialog::Open(InMessageType, InMessage, Title);
#else
	return FMessageDialog::Open(InMessageType, InMessage, &Title);
#endif
}

#undef LOCTEXT_NAMESPACE
