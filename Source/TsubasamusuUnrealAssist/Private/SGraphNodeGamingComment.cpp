// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "SGraphNodeGamingComment.h"

void SGraphNodeGamingComment::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SGraphNodeCustomizableComment::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	UpdateGamingAnimationColor(InDeltaTime);
	UpdateCommentNodeAngle(InDeltaTime);
	UpdateCommentNodeScale(InDeltaTime);
}

void SGraphNodeGamingComment::Construct(const FArguments& InArgs, UEdGraphNode_Comment* InNode)
{
	SGraphNodeCustomizableComment::Construct(SGraphNodeCustomizableComment::FArguments(), InNode);
}

FSlateColor SGraphNodeGamingComment::GetCommentBodyColor() const
{
	return GetGamingAnimationColor();
}

FSlateColor SGraphNodeGamingComment::GetTitleBarColor() const
{
	return GetGamingAnimationColor() * CustomizableCommentNodeDefinitions::TitleBarColorMultiplier;
}

FSlateColor SGraphNodeGamingComment::GetCommentBubbleColor() const
{
	return GetGamingAnimationColor() * CustomizableCommentNodeDefinitions::TitleBarColorMultiplier;
}

void SGraphNodeGamingComment::UpdateGamingAnimationColor(const float InDeltaTime)
{
	GamingAnimationElapsedSeconds += InDeltaTime;
	
	if (GamingAnimationElapsedSeconds > GamingCommentNodeDefinitions::GamingAnimationDuration)
	{
		GamingAnimationElapsedSeconds = 0.0f;
	}
}

void SGraphNodeGamingComment::UpdateCommentNodeScale(const float InDeltaTime)
{
	ScalingAnimationElapsedSeconds = FMath::Fmod(ScalingAnimationElapsedSeconds + InDeltaTime, GamingCommentNodeDefinitions::ScalingAnimationDuration);

	const float Alpha = FMath::Sin(PI * ScalingAnimationElapsedSeconds / GamingCommentNodeDefinitions::ScalingAnimationDuration);

	const float NewScale = FMath::Lerp(1.0f, GamingCommentNodeDefinitions::MaxCommentNodeScale, Alpha);

	SetCommentNodeScale(NewScale);
}

void SGraphNodeGamingComment::UpdateCommentNodeAngle(const float InDeltaTime)
{
	CurrentCommentNodeAngle += InDeltaTime * GamingCommentNodeDefinitions::RotatingAnglePerSeconds;

	SetCommentNodeAngle(CurrentCommentNodeAngle);
}

FLinearColor SGraphNodeGamingComment::GetGamingAnimationColor() const
{
	const float Hue = (GamingAnimationElapsedSeconds / GamingCommentNodeDefinitions::GamingAnimationDuration) * 255.0f;

	return FLinearColor::MakeFromHSV8(static_cast<uint8>(Hue), 255.0f, 255.0f);
}
