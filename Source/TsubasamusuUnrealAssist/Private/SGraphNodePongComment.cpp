// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "SGraphNodePongComment.h"
#include "SDottedLine.h"
#include "SGraphNodeCustomizableComment.h"
#include "TsubasamusuUnrealAssistSettings.h"
#include "Widgets/Layout/SScaleBox.h"

void SGraphNodePongComment::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SGraphNodeCustomizableComment::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();
	
	if (CachedUiColor != TsubasamusuUnrealAssistSettings->PongUiColor)
	{
		CachedUiColor = TsubasamusuUnrealAssistSettings->PongUiColor;

		SetScrollBarStyle(GetDesiredScrollBarStyle());
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

FText SGraphNodePongComment::GetCommentNodeToolTipText() const
{
	return FText::GetEmpty();
}

float SGraphNodePongComment::GetTitleBarLineOpacity() const
{
	return 1.0f;
}

void SGraphNodePongComment::CreateCommentNodeWidget(const FGraphNodeMetaData& InGraphNodeMetaData)
{
	SGraphNodeCustomizableComment::CreateCommentNodeWidget(InGraphNodeMetaData);

	CreateTitleBarAreaWidget();
	CreatePlayAreaWidget();
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

void SGraphNodePongComment::CreateTitleBarAreaWidget()
{
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

void SGraphNodePongComment::CreatePlayAreaWidget()
{
	CreateCenterDottedLine();
	CreateScrollBars();
	CreateScrollBoxes();
	CreateBallImage();
}

void SGraphNodePongComment::CreateCenterDottedLine()
{
	GetCommentNodeContentOverlay()->AddSlot()
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Fill)
	[
		SNew(SDottedLine)
		.Thickness(3.0f)
		.OneCycleLength(8.0f)
		.DottedRatio(0.5f)
		.Color(this, &SGraphNodePongComment::GetDesiredUiColor)
		.Orientation(Orient_Vertical)
	];
}

void SGraphNodePongComment::CreateScrollBars()
{
	const TSharedPtr<SOverlay> PlayAreaOverlay = GetCommentNodeContentOverlay();
	
	PlayAreaOverlay->AddSlot()
	.HAlign(HAlign_Right)
	.VAlign(VAlign_Fill)
	.Padding(0.0f, 3.0f, 10.0f, 3.0f)
	[
		SAssignNew(RightScrollBar, SScrollBar)
		.Orientation(Orient_Vertical)
		.AlwaysShowScrollbar(true)
		.Thickness(FVector2D(8.0f, 8.0f))
	];
	
	PlayAreaOverlay->AddSlot()
	.HAlign(HAlign_Left)
	.VAlign(VAlign_Fill)
	.Padding(10.0f, 3.0f, 0.0f, 3.0f)
	[
		SAssignNew(LeftScrollBar, SScrollBar)
		.Orientation(Orient_Vertical)
		.AlwaysShowScrollbar(true)
		.Thickness(FVector2D(8.0f, 8.0f))
	];

	SetScrollBarStyle(GetDesiredScrollBarStyle());
}

void SGraphNodePongComment::CreateScrollBoxes()
{
	const TSharedPtr<SOverlay> PlayAreaOverlay = GetCommentNodeContentOverlay();
	
	PlayAreaOverlay->AddSlot()
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Top)
	[
		SNew(SBox)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Top)
		.WidthOverride(0.0f)
		[
			SNew(SScrollBox)
			.Orientation(Orient_Vertical)
			.ExternalScrollbar(LeftScrollBar)
			.WheelScrollMultiplier(0.0f)
			+ SScrollBox::Slot()
			[
				SNew(SBox)
				.HeightOverride(this, &SGraphNodePongComment::GetDesiredScrollBoxHeight)
			]
		]
	];

	PlayAreaOverlay->AddSlot()
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Top)
	[
		SNew(SBox)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Top)
		.WidthOverride(0.0f)
		[
			SNew(SScrollBox)
			.Orientation(Orient_Vertical)
			.ExternalScrollbar(RightScrollBar)
			.WheelScrollMultiplier(0.0f)
			+ SScrollBox::Slot()
			[
				SNew(SBox)
				.HeightOverride(this, &SGraphNodePongComment::GetDesiredScrollBoxHeight)
			]
		]
	];
}

void SGraphNodePongComment::CreateBallImage()
{
	GetCommentNodeContentOverlay()->AddSlot()
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Center)
	.Padding(TAttribute<FMargin>::Create(TAttribute<FMargin>::FGetter::CreateSP(this, &SGraphNodePongComment::GetDesiredBallPadding)))
	[
		SAssignNew(BallImage, SImage)
		.Image(FAppStyle::Get().GetBrush(TEXT("UnrealCircle.Thick")))
		.ColorAndOpacity(this, &SGraphNodePongComment::GetDesiredUiColor)
		.DesiredSizeOverride(this, &SGraphNodePongComment::GetDesiredBallImageSize)
	];
}

FSlateColor SGraphNodePongComment::GetDesiredUiColor() const
{
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();

	return TsubasamusuUnrealAssistSettings->PongUiColor;
}

FOptionalSize SGraphNodePongComment::GetDesiredScrollBoxHeight() const
{
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();

	return FOptionalSize(GetPlayAreaSize().Y * (1 / TsubasamusuUnrealAssistSettings->PongSliderLengthMultiplier));
}

FScrollBarStyle SGraphNodePongComment::GetDesiredScrollBarStyle()
{
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();

	const FSlateColorBrush DesiredScrollBarImage = FSlateColorBrush(TsubasamusuUnrealAssistSettings->PongUiColor);
	
	FScrollBarStyle DesiredScrollBarStyle = FScrollBarStyle();
	
	DesiredScrollBarStyle.SetNormalThumbImage(DesiredScrollBarImage);
	DesiredScrollBarStyle.SetHoveredThumbImage(DesiredScrollBarImage);
	DesiredScrollBarStyle.SetDraggedThumbImage(DesiredScrollBarImage);

	return DesiredScrollBarStyle;
}

TOptional<FVector2D> SGraphNodePongComment::GetDesiredBallImageSize() const
{
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();

	return TsubasamusuUnrealAssistSettings->PongBallImageSize;
}

FMargin SGraphNodePongComment::GetDesiredBallPadding() const
{
	return CachedBallPadding;
}

void SGraphNodePongComment::SetScrollBarStyle(const FScrollBarStyle& NewScrollBarStyle)
{
	if (RightScrollBar.IsValid() && LeftScrollBar.IsValid())
	{
		CachedScrollBarStyle = NewScrollBarStyle;
		
		RightScrollBar->SetStyle(&CachedScrollBarStyle);
		LeftScrollBar->SetStyle(&CachedScrollBarStyle);
	}
}

void SGraphNodePongComment::MoveBall(const FVector2D& NormalizedDirection, const float InDeltaTime)
{
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();

	const FVector2D DesiredBallOffset = NormalizedDirection * TsubasamusuUnrealAssistSettings->PongBallSpeed * InDeltaTime;

	SetBallPosition(GetBallPosition() + DesiredBallOffset);
}

void SGraphNodePongComment::SetBallPosition(const FVector2D& NewBallPosition)
{
	CachedBallPadding = FMargin(NewBallPosition.X, 0.0f, 0.0f, NewBallPosition.Y);
}

FVector2D SGraphNodePongComment::GetBallPosition() const
{
	return FVector2D(CachedBallPadding.Left, CachedBallPadding.Bottom);
}

FVector2D SGraphNodePongComment::GetLocalBallCoordinates() const
{
	const FVector2D BallPosition = GetBallPosition();
	
	const float LocalBallCoordinatesX = (GetDesiredSize().X / 2) + BallPosition.X;
	const float LocalBallCoordinatesY = (GetPlayAreaSize().Y / 2) + GetTitleBarHeight() - BallPosition.Y;
	
	return FVector2D(LocalBallCoordinatesX, LocalBallCoordinatesY);
}


FSlateRect SGraphNodePongComment::GetAbsoluteRect(const FGeometry& InFGeometry)
{
	const FVector2D TopLeft = InFGeometry.LocalToAbsolute(FVector2D::ZeroVector);
	const FVector2D BottomRight = InFGeometry.LocalToAbsolute(InFGeometry.GetLocalSize());
	
	return FSlateRect(TopLeft.X, TopLeft.Y, BottomRight.X, BottomRight.Y);
}

FVector2D SGraphNodePongComment::GetPlayAreaSize() const
{
	const FVector2D CommentNodeDesiredSize = GetDesiredSize();
	
	return FVector2D(CommentNodeDesiredSize.X, CommentNodeDesiredSize.Y - GetTitleBarHeight());
}

FText SGraphNodePongComment::GetLeftScoreText() const
{
	return FText::FromString(FString::FromInt(LeftScore));
}

FText SGraphNodePongComment::GetRightScoreText() const
{
	return FText::FromString(FString::FromInt(RightScore));
}
