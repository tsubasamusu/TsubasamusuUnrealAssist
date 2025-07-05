// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "TsubasamusuUnrealAssistSettings.h"
#include "CommentNodeType.h"

UTsubasamusuUnrealAssistSettings::UTsubasamusuUnrealAssistSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bEnableBlueprintSuggestion = false;
	
	CommentNodeType = ECommentNodeType::Normal;
	DeeplApiKey = TEXT("");
	
	GamingColorAnimationDuration = 1.0f;
	GamingScaleAnimationDuration = 1.0f;
	GamingCommentNodeRotatingAnglePerSeconds = 720.0f;
	MaxGamingCommentNodeScale = 1.5f;

	PongCommentNodeColor = FLinearColor::Black;
	PongUiColor = FLinearColor::White;
	PongBallImageSize = FVector2D(30.0f, 30.0f);
	PongSliderLengthMultiplier = 0.3f;
	PongBallSpeed = 100.0f;
	PongIntervalSeconds = 1.0f;
	bMakeLeftScrollBarNPC = false;
}

UTsubasamusuUnrealAssistSettings* UTsubasamusuUnrealAssistSettings::GetSettingsChecked()
{
	UTsubasamusuUnrealAssistSettings* Settings = GetMutableDefault<UTsubasamusuUnrealAssistSettings>();

	check(Settings);

	return Settings;
}
