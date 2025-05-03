// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "SGraphNodePongComment.h"
#include "SGraphNodeCustomizableComment.h"
#include "TsubasamusuUnrealAssistSettings.h"
#include "Widgets/Layout/SScaleBox.h"

void SGraphNodePongComment::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SGraphNodeCustomizableComment::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
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

EMouseCursor::Type SGraphNodePongComment::GetTitleBarMouseCursor() const
{
	if (PlayButton.IsValid())
	{
		if (PlayButton->IsHovered())
		{
			return EMouseCursor::Default;
		}
	}
	
	return EMouseCursor::CardinalCross;
}

FSlateColor SGraphNodePongComment::GetCommentNodeColor() const
{
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();

	SetCommentNodeColor(TsubasamusuUnrealAssistSettings->PongCommentNodeColor);

	return TsubasamusuUnrealAssistSettings->PongCommentNodeColor;
}

FLinearColor SGraphNodePongComment::GetTitleBarLineColor() const
{
	return GetDesiredUiColor().GetSpecifiedColor();
}

float SGraphNodePongComment::GetTitleBarLineOpacity() const
{
	return 1.0f;
}

void SGraphNodePongComment::CreateCommentNodeWidget(const FGraphNodeMetaData& InGraphNodeMetaData)
{
	SGraphNodeCustomizableComment::CreateCommentNodeWidget(InGraphNodeMetaData);
	
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();
	
	GetTitleBarOverlay()->AddSlot()
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	.Padding(0, 3, 0, 3)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SScaleBox)
			.Stretch(EStretch::ScaleToFitY)
			[
				SNew(STextBlock)
				.Text(this, &SGraphNodePongComment::GetLeftScoreText)
				.ColorAndOpacity(this, &SGraphNodePongComment::GetDesiredUiColor)
			]
		]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Fill)
		.AutoWidth()
		[
			SNew(SScaleBox)
			.Stretch(EStretch::ScaleToFitY)
			[
				SAssignNew(PlayButton, SButton)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.ContentPadding(0)
				.ButtonStyle(FAppStyle::Get(), "NoBorder")
				.OnClicked(this, &SGraphNodePongComment::OnClickedButton)
				[
					SAssignNew(PlayButtonImage, SImage)
					.ColorAndOpacity(this, &SGraphNodePongComment::GetDesiredUiColor)
				]
			]
		]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SScaleBox)
			.Stretch(EStretch::ScaleToFitY)
			[
				SNew(STextBlock)
				.Text(this, &SGraphNodePongComment::GetRightScoreText)
				.ColorAndOpacity(this, &SGraphNodePongComment::GetDesiredUiColor)
			]
		]
	];

	ChangePlayButtonImage(true);
	ChangePlayButtonToolTipText(true);
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

FSlateColor SGraphNodePongComment::GetDesiredUiColor() const
{
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();

	return TsubasamusuUnrealAssistSettings->PongUiColor;
}

FText SGraphNodePongComment::GetLeftScoreText() const
{
	return FText::FromString(FString::FromInt(LeftScore));
}

FText SGraphNodePongComment::GetRightScoreText() const
{
	return FText::FromString(FString::FromInt(RightScore));
}
