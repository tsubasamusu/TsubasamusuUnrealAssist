// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "SGraphNodePongComment.h"
#include "SGraphNodeCustomizableComment.h"
#include "TsubasamusuUnrealAssistSettings.h"

void SGraphNodePongComment::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SGraphNodeCustomizableComment::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();
	
	if (GetCommentNodeColor() != TsubasamusuUnrealAssistSettings->PongCommentNodeColor)
	{
		SetCommentNodeColor(TsubasamusuUnrealAssistSettings->PongCommentNodeColor);
	}
}

void SGraphNodePongComment::Construct(const FArguments& InArgs, UEdGraphNode_Comment* InNode)
{
	SGraphNodeCustomizableComment::Construct(SGraphNodeCustomizableComment::FArguments(), InNode);
}

bool SGraphNodePongComment::CanBeSelected(const FVector2D& MousePositionInNode) const
{
	return true;
}

EVisibility SGraphNodePongComment::GetCommentTextVisibility() const
{
	return EVisibility::Hidden;
}

EVisibility SGraphNodePongComment::GetCommentBubbleVisibility() const
{
	return EVisibility::Collapsed;
}


FReply SGraphNodePongComment::OnClickedButton()
{
	if (bIsPlaying)
	{
		StopGame();
	}
	else
	{
		StartGame();
	}

	bIsPlaying = !bIsPlaying;

	return FReply::Handled();
}

const FSlateBrush* SGraphNodePongComment::GetStartIcon()
{
	return FAppStyle::Get().GetBrush(TEXT("Animation.Forward"));
}

const FSlateBrush* SGraphNodePongComment::GetStopIcon()
{
	return FAppStyle::Get().GetBrush(TEXT("Animation.Pause"));
}

void SGraphNodePongComment::StartGame()
{
}

void SGraphNodePongComment::StopGame()
{
}

