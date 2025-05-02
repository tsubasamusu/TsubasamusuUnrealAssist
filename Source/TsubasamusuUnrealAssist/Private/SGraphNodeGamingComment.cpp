// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "SGraphNodeGamingComment.h"
#include "TsubasamusuUnrealAssistSettings.h"

void SGraphNodeGamingComment::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SGraphNodeCustomizableComment::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	UpdateCommentNodeColor(InDeltaTime);
	UpdateCommentNodeAngle(InDeltaTime);
	UpdateCommentNodeScale(InDeltaTime);
}

void SGraphNodeGamingComment::Construct(const FArguments& InArgs, UEdGraphNode_Comment* InNode)
{
	SGraphNodeCustomizableComment::Construct(SGraphNodeCustomizableComment::FArguments(), InNode);
}

FSlateColor SGraphNodeGamingComment::GetCommentBodyColor() const
{
	return GetDesiredGamingColor();
}

FSlateColor SGraphNodeGamingComment::GetTitleBarColor() const
{
	return GetDesiredGamingColor() * CustomizableCommentNodeDefinitions::TitleBarColorMultiplier;
}

FSlateColor SGraphNodeGamingComment::GetCommentBubbleColor() const
{
	return GetDesiredGamingColor() * CustomizableCommentNodeDefinitions::TitleBarColorMultiplier;
}

void SGraphNodeGamingComment::UpdateCommentNodeColor(const float InDeltaTime)
{
	ColorAnimationElapsedSeconds += InDeltaTime;

	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();
	
	if (ColorAnimationElapsedSeconds > TsubasamusuUnrealAssistSettings->ColorAnimationDuration)
	{
		ColorAnimationElapsedSeconds = 0.0f;
	}
}

void SGraphNodeGamingComment::UpdateCommentNodeScale(const float InDeltaTime)
{
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();
	
	ScaleAnimationElapsedSeconds = FMath::Fmod(ScaleAnimationElapsedSeconds + InDeltaTime, TsubasamusuUnrealAssistSettings->ScaleAnimationDuration);

	const float Alpha = FMath::Sin(PI * ScaleAnimationElapsedSeconds / TsubasamusuUnrealAssistSettings->ScaleAnimationDuration);

	const float NewScale = FMath::Lerp(1.0f, TsubasamusuUnrealAssistSettings->MaxCommentNodeScale, Alpha);

	SetCommentNodeScale(NewScale);
}

void SGraphNodeGamingComment::UpdateCommentNodeAngle(const float InDeltaTime)
{
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();
	
	CurrentCommentNodeAngle += InDeltaTime * TsubasamusuUnrealAssistSettings->RotatingAnglePerSeconds;

	SetCommentNodeAngle(CurrentCommentNodeAngle);
}

FLinearColor SGraphNodeGamingComment::GetDesiredGamingColor() const
{
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();
	
	const float Hue = (ColorAnimationElapsedSeconds / TsubasamusuUnrealAssistSettings->ColorAnimationDuration) * 255.0f;

	return FLinearColor::MakeFromHSV8(static_cast<uint8>(Hue), 255.0f, 255.0f);
}
