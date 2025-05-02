// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "TsubasamusuUnrealAssistSettings.h"
#include "CommentNodeType.h"

UTsubasamusuUnrealAssistSettings::UTsubasamusuUnrealAssistSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CommentNodeType = ECommentNodeType::Normal;
	
	GamingColorAnimationDuration = 1.0f;
	GamingScaleAnimationDuration = 1.0f;
	GamingCommentNodeRotatingAnglePerSeconds = 720.0f;
	MaxGamingCommentNodeScale = 1.5f;

	PongCommentNodeColor = FLinearColor::Black;
}

UTsubasamusuUnrealAssistSettings* UTsubasamusuUnrealAssistSettings::GetSettingsChecked()
{
	UTsubasamusuUnrealAssistSettings* Settings = GetMutableDefault<UTsubasamusuUnrealAssistSettings>();

	check(Settings);

	return Settings;
}
