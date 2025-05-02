// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "SGraphNodeGamingComment.h"
#include "EdGraphNode_Comment.h"
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

void SGraphNodeGamingComment::UpdateCommentNodeColor(const float InDeltaTime)
{
	ColorAnimationElapsedSeconds += InDeltaTime;

	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();
	
	if (ColorAnimationElapsedSeconds > TsubasamusuUnrealAssistSettings->GamingColorAnimationDuration)
	{
		ColorAnimationElapsedSeconds = 0.0f;
	}
	
	const float Hue = (ColorAnimationElapsedSeconds / TsubasamusuUnrealAssistSettings->GamingColorAnimationDuration) * 255.0f;

	UEdGraphNode_Comment* CommentNode = CastChecked<UEdGraphNode_Comment>(GraphNode);

	CommentNode->CommentColor = FLinearColor::MakeFromHSV8(static_cast<uint8>(Hue), 255.0f, 255.0f);
}

void SGraphNodeGamingComment::UpdateCommentNodeScale(const float InDeltaTime)
{
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();
	
	ScaleAnimationElapsedSeconds = FMath::Fmod(ScaleAnimationElapsedSeconds + InDeltaTime, TsubasamusuUnrealAssistSettings->GamingScaleAnimationDuration);

	const float Alpha = FMath::Sin(PI * ScaleAnimationElapsedSeconds / TsubasamusuUnrealAssistSettings->GamingScaleAnimationDuration);

	const float NewScale = FMath::Lerp(1.0f, TsubasamusuUnrealAssistSettings->MaxGamingCommentNodeScale, Alpha);

	SetCommentNodeScale(NewScale);
}

void SGraphNodeGamingComment::UpdateCommentNodeAngle(const float InDeltaTime)
{
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();
	
	CurrentCommentNodeAngle += InDeltaTime * TsubasamusuUnrealAssistSettings->GamingCommentNodeRotatingAnglePerSeconds;

	SetCommentNodeAngle(CurrentCommentNodeAngle);
}
