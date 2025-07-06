// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "Setting/TsubasamusuUnrealAssistSettings.h"
#include "Internationalization/Culture.h"

UTsubasamusuUnrealAssistSettings::UTsubasamusuUnrealAssistSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bEnableNodeSuggestion = false;
	
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

	ApiKey = TEXT("");
	GptModelName = TEXT("gpt-4");
	GptLanguageCultureName = FInternationalization::Get().GetCurrentLanguage()->GetName();
	bIgnoreNodesDoNotHaveConnectedPins = true;
	bIgnoreCommentNodes = false;
	CommentGenerationConditions = { TEXT("answer briefly") };
}

FCulturePtr UTsubasamusuUnrealAssistSettings::GetGptLanguageCulture() const
{
	return FInternationalization::Get().GetCulture(GptLanguageCultureName);
}

void UTsubasamusuUnrealAssistSettings::SetGptLanguageCulture(const FCulturePtr& InGptLanguageCulture)
{
	GptLanguageCultureName = InGptLanguageCulture->GetName();
	SaveConfig();
}

UTsubasamusuUnrealAssistSettings* UTsubasamusuUnrealAssistSettings::GetSettingsChecked()
{
	UTsubasamusuUnrealAssistSettings* Settings = GetMutableDefault<UTsubasamusuUnrealAssistSettings>();
	check(Settings);
	return Settings;
}
