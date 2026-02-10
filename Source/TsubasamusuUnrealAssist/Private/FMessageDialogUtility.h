// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

class FMessageDialogUtility final
{
public:
	static EAppReturnType::Type OpenWarningMessageDialog(const EAppMsgType::Type InMessageType, const FText& InMessage);
};
