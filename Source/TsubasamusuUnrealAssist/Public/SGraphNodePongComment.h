// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphNodeCustomizableComment.h"

enum class EOverflowBallSide : uint8
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

	//~ Begin SNodePanel::SNode Interface
	virtual bool CanBeSelected(const FVector2D& MousePositionInNode) const override;
	//~ End SNodePanel::SNode Interface
	
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
	void StopGame();

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

	void MoveBall(const FVector2D& NormalizedDirection, const float InDeltaTime);
	
	void SetBallPosition(const FVector2D& NewBallPosition);
	FVector2D GetBallPosition() const;

	FVector2D GetLocalBallCoordinates() const;

	EOverflowBallSide GetOverflowBallSide() const;
	bool BallIsInPlayArea() const;
	static FSlateRect GetAbsoluteRect(const FGeometry& InFGeometry);
	
	FVector2D GetPlayAreaSize() const;
	
	FText GetLeftScoreText() const;
	FText GetRightScoreText() const;
	
	TSharedPtr<SButton> PlayButton;
	
	TSharedPtr<SImage> PlayButtonImage;
	TSharedPtr<SImage> BallImage;

	TSharedPtr<SScrollBar> LeftScrollBar;
	TSharedPtr<SScrollBar> RightScrollBar;
	
	int32 LeftScore;
	int32 RightScore;
	
	bool bIsPlaying;

	FLinearColor CachedUiColor;
	FScrollBarStyle CachedScrollBarStyle;
	FMargin CachedBallPadding;
};
