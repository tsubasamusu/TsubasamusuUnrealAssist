// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "TsubasamusuUnrealAssistSettings.h"
#include "CommentNodeType.h"

UTsubasamusuUnrealAssistSettings::UTsubasamusuUnrealAssistSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CommentNodeType = ECommentNodeType::Normal;
	
	ColorAnimationDuration = 1.0f;
	ScaleAnimationDuration = 1.0f;
	RotatingAnglePerSeconds = 720.0f;
	MaxCommentNodeScale = 1.5f;
}

UTsubasamusuUnrealAssistSettings* UTsubasamusuUnrealAssistSettings::GetSettingsChecked()
{
	UTsubasamusuUnrealAssistSettings* Settings = GetMutableDefault<UTsubasamusuUnrealAssistSettings>();

	check(Settings);

	return Settings;
}
