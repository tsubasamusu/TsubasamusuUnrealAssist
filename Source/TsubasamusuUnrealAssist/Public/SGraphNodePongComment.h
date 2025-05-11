// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphNodeCustomizableComment.h"

enum class EBallSide : uint8
{
	None,
	Left,
	Right,
	Top,
	Bottom
};

class TSUBASAMUSUUNREALASSIST_API SGraphNodePongComment final : public SGraphNodeCustomizableComment
{
	typedef SGraphNodePongComment Super;

public:
	
	SLATE_BEGIN_ARGS(SGraphNodePongComment) {}
	SLATE_END_ARGS()
	
	//~ Begin SWidget Interface
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	//~ End SWidget Interface
	
	void Construct(const FArguments& InArgs, UEdGraphNode_Comment* InNode);

protected:

	//~ Begin SGraphNodeCustomizableComment Interface
	virtual EVisibility GetCommentTextVisibility() const override;
	virtual EVisibility GetCommentBubbleVisibility() const override;
	virtual EMouseCursor::Type GetTitleBarMouseCursor() const override;
	virtual FSlateColor GetCommentNodeColor() const override;
	virtual FLinearColor GetTitleBarLineColor() const override;
	virtual FText GetCommentNodeToolTipText() const override;
	virtual float GetTitleBarLineOpacity() const override;
	virtual void CreateCommentNodeWidget(const FGraphNodeMetaData& InGraphNodeMetaData) override;
	//~ End SGraphNodeCustomizableComment Interface

private:

	const static FSlateBrush* GetStartIcon();
	const static FSlateBrush* GetStopIcon();

	FReply OnClickedButton();
	
	void StartGame();
	void EndGame();

	void ChangePlayButtonImage(const bool bChangeToStartImage) const;
	void ChangePlayButtonToolTipText(const bool bChangeToStartToolTipText) const;

	void CreateTitleBarAreaWidget();
	void CreatePlayAreaWidget();
	void CreateCenterDottedLine();
	void CreateScrollBars();
	void CreateScrollBoxes();
	void CreateBallImage();
	
	FSlateColor GetDesiredUiColor() const;
	FOptionalSize GetDesiredScrollBoxHeight() const;
	static FScrollBarStyle GetDesiredScrollBarStyle();
	TOptional<FVector2D> GetDesiredBallImageSize() const;
	FMargin GetDesiredBallPadding() const;

	void SetScrollBarStyle(const FScrollBarStyle& NewScrollBarStyle);

	float GetDesiredLeftScrollBarThumbPositionY() const;
	
	FVector2D GetDesiredBallMovementDirection();
	FVector2D GetDesiredInitialBallMovementDirection() const;
	
	void MoveBall(const FVector2D& MovementDirection, const float InDeltaTime);

	void ResetBallPosition();
	void SetBallPosition(const FVector2D& NewBallPosition);
	FVector2D GetBallPosition() const;

	FVector2D GetLocalBallCoordinates() const;

	EBallSide GetOverflowPlayAreaBallSide() const;
	EBallSide GetHitScrollBarThumbBallSide() const;
	
	bool BallIsInPlayArea() const;
	bool BallIsInScrollBarThumb(const TSharedPtr<SScrollBar> InScrollBar) const;

	FSlateRect GetBallImageRect() const;
	static FSlateRect GetAbsoluteRect(const TSharedPtr<SWidget> InWidget);
	static FSlateRect GetScrollBarThumbRect(const TSharedPtr<SScrollBar> InScrollBar);

	static FVector2D GetWidgetRectSize(const TSharedPtr<SWidget> InWidget);
	static FVector2D GetWidgetRectSize(const FSlateRect InWidgetRect);
	
	FVector2D GetPlayAreaSize() const;

	void SetBallImageVisibility(const bool bNewVisibility) const;
	
	void SetLeftScrollBarEnabled(const bool bEnabled) const;
	bool LeftScrollBarIsEnabled() const;
	
	bool GameIsInInterval() const;

	void OnLeftScored();
	void OnRightScored();
	void OnSomeoneScored();
	
	FText GetLeftScoreText() const;
	FText GetRightScoreText() const;
	
	TSharedPtr<SButton> PlayButton;
	
	TSharedPtr<SImage> PlayButtonImage;
	TSharedPtr<SImage> BallImage;

	TSharedPtr<SScrollBar> LeftScrollBar;
	TSharedPtr<SScrollBar> RightScrollBar;
	
	int32 LeftScore;
	int32 RightScore;

	bool bLeftIsLastScoredPlayer;
	bool bIsPlaying;
	float IntervalSeconds;

	float CachedDeltaTime;
	FLinearColor CachedUiColor;
	FScrollBarStyle CachedScrollBarStyle;
	FMargin CachedBallPadding;
	FVector2D CachedBallMovementDirection;
};
