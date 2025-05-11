// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "SGraphNodePongComment.h"
#include "SDottedLine.h"
#include "SGraphNodeCustomizableComment.h"
#include "TsubasamusuUnrealAssistSettings.h"
#include "Widgets/Layout/SScaleBox.h"

void SGraphNodePongComment::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SGraphNodeCustomizableComment::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	CachedDeltaTime = InDeltaTime;
	
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();
	
	if (CachedUiColor != TsubasamusuUnrealAssistSettings->PongUiColor)
	{
		CachedUiColor = TsubasamusuUnrealAssistSettings->PongUiColor;

		SetScrollBarStyle(GetDesiredScrollBarStyle());
	}

	if (LeftScrollBarIsEnabled() == TsubasamusuUnrealAssistSettings->bMakeLeftScrollBarNPC)
	{
		SetLeftScrollBarEnabled(!TsubasamusuUnrealAssistSettings->bMakeLeftScrollBarNPC);
	}

	if (GameIsInInterval())
	{
		IntervalSeconds += InDeltaTime;

		if (IntervalSeconds >= TsubasamusuUnrealAssistSettings->PongIntervalSeconds)
		{
			IntervalSeconds = 0.0f;
			
			SetBallImageVisibility(true);
		}
	}
	
	MoveBall(GetDesiredBallMovementDirection(), InDeltaTime);
}

void SGraphNodePongComment::Construct(const FArguments& InArgs, UEdGraphNode_Comment* InNode)
{
	SGraphNodeCustomizableComment::Construct(SGraphNodeCustomizableComment::FArguments(), InNode);
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
	if (bIsPlaying)
	{
		EndGame();
	}
	else
	{
		StartGame();
	}
	
	return FReply::Handled();
}

void SGraphNodePongComment::StartGame()
{
	bIsPlaying = true;
	
	ChangePlayButtonImage(false);
	ChangePlayButtonToolTipText(false);
	SetBallImageVisibility(true);
}

void SGraphNodePongComment::EndGame()
{
	bIsPlaying = false;
	
	ResetBallPosition();
	ChangePlayButtonImage(true);
	ChangePlayButtonToolTipText(true);
	SetBallImageVisibility(false);

	LeftScore = 0;
	RightScore = 0;
}

const FSlateBrush* SGraphNodePongComment::GetStartIcon()
{
	return FAppStyle::Get().GetBrush(TEXT("Animation.Forward"));
}

const FSlateBrush* SGraphNodePongComment::GetStopIcon()
{
	return FAppStyle::Get().GetBrush(TEXT("Animation.Pause"));
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
	.HAlign(HAlign_Left)
	.VAlign(VAlign_Fill)
	.Padding(10.0f, 3.0f, 0.0f, 3.0f)
	[
		SAssignNew(LeftScrollBar, SScrollBar)
		.Orientation(Orient_Vertical)
		.AlwaysShowScrollbar(true)
		.Thickness(FVector2D(8.0f, 8.0f))
	];

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

	SetBallImageVisibility(false);
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

void SGraphNodePongComment::SetLeftScrollBarThumbPositionY(const float NewPositionY)
{
	CachedLeftScrollBarThumbPositionY = NewPositionY;
	
	const FSlateRect LeftScrollBarThumbRect = GetScrollBarThumbRect(LeftScrollBar);
	const FVector2D LeftScrollBarThumbSize = GetWidgetRectSize(LeftScrollBarThumbRect);

	const TSharedPtr<SOverlay> PlayAreaOverlay = GetCommentNodeContentOverlay();
	const FVector2D PlayAreaSize = GetWidgetRectSize(PlayAreaOverlay);
	
	const float DesiredThumbTopSpaceFraction = (PlayAreaSize.Y - (NewPositionY + (LeftScrollBarThumbSize.Y / 2.0f))) / PlayAreaSize.Y;

	LeftScrollBar->SetState(DesiredThumbTopSpaceFraction, LeftScrollBar->ThumbSizeFraction());
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

float SGraphNodePongComment::GetPredictedBallHitPositionY() const
{
	const TSharedPtr<SOverlay> PlayAreaOverlay = GetCommentNodeContentOverlay();
	const FSlateRect PlayAreaRect = GetAbsoluteRect(PlayAreaOverlay);
	const FVector2D PlayAreaSize = GetWidgetRectSize(PlayAreaRect);

	const FSlateRect BallRect = GetBallImageRect();
	const FVector2D BallSize = GetWidgetRectSize(BallRect);
	const FVector2D BallRadius = BallSize / 2.0f;

	const FSlateRect LeftScrollBarThumbRect = GetScrollBarThumbRect(LeftScrollBar);
	const FSlateRect RightScrollBarThumbRect = GetScrollBarThumbRect(RightScrollBar);

	const float ActualBallMovableRangeX = PlayAreaSize.X - (LeftScrollBarThumbRect.Right - PlayAreaRect.Left) - (PlayAreaRect.Right - RightScrollBarThumbRect.Left);
	const FVector2D ActualBallMovableRange = FVector2D(ActualBallMovableRangeX, PlayAreaSize.Y);
	
	const float BallCenterPositionX = (BallRect.Left + BallRadius.X) - PlayAreaRect.Left;
	const float BallCenterPositionY = PlayAreaRect.Bottom - (BallRect.Bottom - BallRadius.Y);
	const FVector2D BallCenterPosition = FVector2D(BallCenterPositionX, BallCenterPositionY);

	FVector2D BallLeftPosition = FVector2D(BallCenterPosition.X - BallRadius.X, BallCenterPosition.Y);
	const FVector2D BallBottomPosition = FVector2D(BallCenterPosition.X, BallCenterPosition.Y - BallRadius.Y);

	const FVector2D BallEdgeMovableRange = FVector2D(ActualBallMovableRange.X - BallSize.X, ActualBallMovableRange.Y - BallSize.Y);
	const FVector2D DoubledBallEdgeMovableRange = BallEdgeMovableRange * 2.0f;
	
	FVector2D NormalizedBallDirection = CachedBallMovementDirection.GetSafeNormal();

	if (NormalizedBallDirection.X > 0.0f)
	{
		BallLeftPosition.X = 2.0f * BallEdgeMovableRange.X - BallLeftPosition.X;
		NormalizedBallDirection.X *= -1.0f;
	}

	const float BallDirectionTangent = NormalizedBallDirection.Y / NormalizedBallDirection.X;
	
	const float NotReboundedBallPositionY = BallBottomPosition.Y - (BallLeftPosition.X * BallDirectionTangent);
	float ReboundedBallPositionY = FMath::Fmod(NotReboundedBallPositionY, DoubledBallEdgeMovableRange.Y);
	
	if (ReboundedBallPositionY < 0.0f)
	{
		ReboundedBallPositionY += DoubledBallEdgeMovableRange.Y;
	}

	return BallEdgeMovableRange.Y - FMath::Abs(ReboundedBallPositionY - BallEdgeMovableRange.Y) + BallRadius.Y;
}

float SGraphNodePongComment::GetDesiredLeftScrollBarThumbPositionY() const
{
	const TSharedPtr<SOverlay> PlayAreaOverlay = GetCommentNodeContentOverlay();
	const FSlateRect PlayAreaRect = GetAbsoluteRect(PlayAreaOverlay);
	const FVector2D PlayAreaSize = GetWidgetRectSize(PlayAreaRect);

	if (!bIsPlaying)
	{
		return PlayAreaSize.Y / 2.0f;
	}
	
	if (CachedBallMovementDirection.X > 0.0f)
	{
		return CachedLeftScrollBarThumbPositionY;
	}

	return GetPredictedBallHitPositionY();
}

FVector2D SGraphNodePongComment::GetDesiredBallMovementDirection()
{
	if (!bIsPlaying)
	{
		return FVector2D::ZeroVector;
	}

	if (GameIsInInterval())
	{
		return FVector2D::ZeroVector;
	}
	
	if (CachedBallMovementDirection.IsZero())
	{
		return GetDesiredInitialBallMovementDirection();
	}

	FVector2D DesiredBallMovementDirection = CachedBallMovementDirection;  

	if (BallIsInPlayArea())
	{
		if (BallIsInScrollBarThumb(RightScrollBar) && CachedBallMovementDirection.X > 0.0f)
		{
			DesiredBallMovementDirection.X *= -1.0f;
		}
		else if (BallIsInScrollBarThumb(LeftScrollBar) && CachedBallMovementDirection.X < 0.0f)
		{
			DesiredBallMovementDirection.X *= -1.0f;
		}
	}
	else
	{
		const EBallSide OverflowPlayAreaBallSide = GetOverflowPlayAreaBallSide();
	
		if (OverflowPlayAreaBallSide == EBallSide::Right && CachedBallMovementDirection.X > 0.0f)
		{
			OnLeftScored();
		}
		else if (OverflowPlayAreaBallSide == EBallSide::Left && CachedBallMovementDirection.X < 0.0f)
		{
			OnRightScored();
		}
		if (OverflowPlayAreaBallSide == EBallSide::Top && CachedBallMovementDirection.Y > 0.0f)
		{
			DesiredBallMovementDirection.Y *= -1.0f;
		}
		else if (OverflowPlayAreaBallSide == EBallSide::Bottom && CachedBallMovementDirection.Y < 0.0f)
		{
			DesiredBallMovementDirection.Y *= -1.0f;
		}
	}

	return DesiredBallMovementDirection.GetSafeNormal();
}

FVector2D SGraphNodePongComment::GetDesiredInitialBallMovementDirection() const
{
	const float DirectionX = bLeftIsLastScoredPlayer ? 1.0f : -1.0f;
	const float DirectionY = FMath::RandBool() ? 1.0f : -1.0f;

	FVector2D Direction = FVector2D(DirectionX, DirectionY);
	Direction.Normalize();

	return Direction;
}

void SGraphNodePongComment::MoveBall(const FVector2D& MovementDirection, const float InDeltaTime)
{
	if (!BallImage.IsValid())
	{
		return;
	}
	
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();

	const FVector2D DesiredBallOffset = MovementDirection * TsubasamusuUnrealAssistSettings->PongBallSpeed * InDeltaTime;

	SetBallPosition(GetBallPosition() + DesiredBallOffset);

	CachedBallMovementDirection = MovementDirection;
}

void SGraphNodePongComment::ResetBallPosition()
{
	SetBallPosition(FVector2D::ZeroVector);
	
	CachedBallMovementDirection = FVector2D::ZeroVector;
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

EBallSide SGraphNodePongComment::GetOverflowPlayAreaBallSide() const
{
	const TSharedPtr<SOverlay> PlayAreaOverlay = GetCommentNodeContentOverlay();
	
	if (!BallImage.IsValid() || !PlayAreaOverlay.IsValid())
	{
		return EBallSide::None;
	}
	
	const FSlateRect BallRect = GetAbsoluteRect(BallImage);
	const FSlateRect PlayAreaRect = GetAbsoluteRect(PlayAreaOverlay);

	if (BallRect.Left < PlayAreaRect.Left)
	{
		return EBallSide::Left;
	}
	
	if (BallRect.Right > PlayAreaRect.Right)
	{
		return EBallSide::Right;
	}

	if (BallRect.Top < PlayAreaRect.Top)
	{
		return EBallSide::Top;
	}

	if (BallRect.Bottom > PlayAreaRect.Bottom)
	{
		return EBallSide::Bottom;
	}
	
	return EBallSide::None;
}

EBallSide SGraphNodePongComment::GetHitScrollBarThumbBallSide() const
{
	if (!BallImage.IsValid() || !RightScrollBar.IsValid() || !LeftScrollBar.IsValid())
	{
		return EBallSide::None;
	}

	if (BallIsInScrollBarThumb(LeftScrollBar))
	{
		return EBallSide::Left;
	}
	
	if (BallIsInScrollBarThumb(RightScrollBar))
	{
		return EBallSide::Right;
	}

	return EBallSide::None;
}

bool SGraphNodePongComment::BallIsInPlayArea() const
{
	const TSharedPtr<SOverlay> PlayAreaOverlay = GetCommentNodeContentOverlay();
	
	if (BallImage.IsValid() && PlayAreaOverlay.IsValid())
	{
		const FSlateRect BallRect = GetAbsoluteRect(BallImage);
		const FSlateRect PlayAreaRect = GetAbsoluteRect(PlayAreaOverlay);

		return FSlateRect::IsRectangleContained(PlayAreaRect, BallRect);
	}

	return true;
}

bool SGraphNodePongComment::BallIsInScrollBarThumb(const TSharedPtr<SScrollBar> InScrollBar) const
{
	if (BallImage.IsValid())
	{
		return FSlateRect::DoRectanglesIntersect(GetBallImageRect(), GetScrollBarThumbRect(InScrollBar));
	}
	
	return false;
}

FSlateRect SGraphNodePongComment::GetBallImageRect() const
{
	if (BallImage.IsValid())
	{
		return GetAbsoluteRect(BallImage);
	}

	return FSlateRect();
}

FSlateRect SGraphNodePongComment::GetAbsoluteRect(const TSharedPtr<SWidget> InWidget)
{
	const FGeometry& Geometry = InWidget->GetCachedGeometry();
	
	const FVector2D TopLeft = Geometry.LocalToAbsolute(FVector2D::ZeroVector);
	const FVector2D BottomRight = Geometry.LocalToAbsolute(Geometry.GetLocalSize());
	
	return FSlateRect(TopLeft.X, TopLeft.Y, BottomRight.X, BottomRight.Y);
}

FSlateRect SGraphNodePongComment::GetScrollBarThumbRect(const TSharedPtr<SScrollBar> InScrollBar)
{
	const FSlateRect ScrollBarRect = GetAbsoluteRect(InScrollBar);
	
	const float ScrollBarLength = ScrollBarRect.Bottom - ScrollBarRect.Top;
	const float ThumbTopSpace = ScrollBarLength * InScrollBar->DistanceFromTop();
	const float ThumbBottomSpace = ScrollBarLength * InScrollBar->DistanceFromBottom();
	
	return FSlateRect(ScrollBarRect.Left, ScrollBarRect.Top + ThumbTopSpace, ScrollBarRect.Right, ScrollBarRect.Bottom - ThumbBottomSpace);
}

FVector2D SGraphNodePongComment::GetWidgetRectSize(const TSharedPtr<SWidget> InWidget)
{
	const FSlateRect WidgetRect = GetAbsoluteRect(InWidget);

	return GetWidgetRectSize(WidgetRect);
}

FVector2D SGraphNodePongComment::GetWidgetRectSize(const FSlateRect InWidgetRect)
{
	return FVector2D(InWidgetRect.Right - InWidgetRect.Left, InWidgetRect.Bottom - InWidgetRect.Top);
}

FVector2D SGraphNodePongComment::GetPlayAreaSize() const
{
	const FVector2D CommentNodeDesiredSize = GetDesiredSize();
	
	return FVector2D(CommentNodeDesiredSize.X, CommentNodeDesiredSize.Y - GetTitleBarHeight());
}

void SGraphNodePongComment::SetBallImageVisibility(const bool bNewVisibility) const
{
	if (BallImage.IsValid())
	{
		BallImage->SetVisibility(bNewVisibility ? EVisibility::Visible : EVisibility::Hidden);
	}
}

void SGraphNodePongComment::SetLeftScrollBarEnabled(const bool bEnabled) const
{
	if (LeftScrollBar.IsValid())
	{
		LeftScrollBar->SetVisibility(bEnabled ? EVisibility::Visible : EVisibility::HitTestInvisible);
	}
}

bool SGraphNodePongComment::LeftScrollBarIsEnabled() const
{
	if (LeftScrollBar.IsValid())
	{
		return LeftScrollBar->GetVisibility() == EVisibility::Visible;
	}

	return false;
}

bool SGraphNodePongComment::GameIsInInterval() const
{
	return IntervalSeconds > 0.0f;
}

void SGraphNodePongComment::OnLeftScored()
{
	LeftScore++;
	bLeftIsLastScoredPlayer = true;
	
	OnSomeoneScored();
}

void SGraphNodePongComment::OnRightScored()
{
	RightScore++;
	bLeftIsLastScoredPlayer = false;
	
	OnSomeoneScored();
}

void SGraphNodePongComment::OnSomeoneScored()
{
	ResetBallPosition();
	SetBallImageVisibility(false);
	
	IntervalSeconds = CachedDeltaTime;
}

FText SGraphNodePongComment::GetLeftScoreText() const
{
	return FText::FromString(FString::FromInt(LeftScore));
}

FText SGraphNodePongComment::GetRightScoreText() const
{
	return FText::FromString(FString::FromInt(RightScore));
}
