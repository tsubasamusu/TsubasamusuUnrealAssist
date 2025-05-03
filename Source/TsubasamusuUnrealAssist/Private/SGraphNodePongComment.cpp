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
	ChangePlayButtonImage(bIsPlaying);
	ChangePlayButtonToolTipText(bIsPlaying);
	
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

void SGraphNodePongComment::ChangePlayButtonImage(const bool bChangeToStartImage) const
{
	if (PlayButtonImage.IsValid())
	{
		const FSlateBrush* NewImage = bChangeToStartImage ? GetStartIcon() : GetStopIcon();
		
		PlayButtonImage->SetImage(NewImage);
	}
}

void SGraphNodePongComment::ChangePlayButtonToolTipText(const bool bChangeToStartToolTipText) const
{
	if (PlayButton.IsValid())
	{
		const FText NewToolTipText = FText::FromString(bChangeToStartToolTipText ? TEXT("Start game") : TEXT("Stop game"));
		
		PlayButton->SetToolTipText(NewToolTipText);
	}
}

void SGraphNodePongComment::SetAllUiColor(const FSlateColor& NewColor) const
{
	SetPlayButtonImageColor(NewColor);
}

void SGraphNodePongComment::SetPlayButtonImageColor(const FSlateColor& NewColor) const
{
	if (PlayButtonImage.IsValid())
	{
		PlayButtonImage->SetColorAndOpacity(NewColor);
	}
}
