// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphNodeCustomizableComment.h"

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
	//~ End SGraphNodeCustomizableComment Interface

private:

	const static FSlateBrush* GetStartIcon();
	const static FSlateBrush* GetStopIcon();

	FReply OnClickedButton();
	
	void StartGame();
	void StopGame();

	void ChangePlayButtonImage(const bool bChangeToStartImage) const;
	void ChangePlayButtonToolTipText(const bool bChangeToStartToolTipText) const;
	
	bool bIsPlaying;

	TSharedPtr<SButton> PlayButton;
	TSharedPtr<SImage> PlayButtonImage;
};
