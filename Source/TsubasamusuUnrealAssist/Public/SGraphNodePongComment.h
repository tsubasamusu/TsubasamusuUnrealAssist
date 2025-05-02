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

	//~ Begin SGraphNodeCustomizableComment Interface
	virtual EVisibility GetCommentTextVisibility() const override;
	virtual EVisibility GetCommentBubbleVisibility() const override;
	virtual int32 GetCommentFontSize() const override;
	virtual FMargin GetTitleBarPadding() const override;
	//~ End SGraphNodeCustomizableComment Interface
};
