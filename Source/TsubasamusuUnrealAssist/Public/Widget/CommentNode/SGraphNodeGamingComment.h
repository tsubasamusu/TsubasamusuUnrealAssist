// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphNodeCustomizableComment.h"

class TSUBASAMUSUUNREALASSIST_API SGraphNodeGamingComment final : public SGraphNodeCustomizableComment
{
	typedef SGraphNodeGamingComment Super;

public:
	
	SLATE_BEGIN_ARGS(SGraphNodeGamingComment) {}
	SLATE_END_ARGS()
	
	//~ Begin SWidget Interface
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	//~ End SWidget Interface
	
	void Construct(const FArguments& InArgs, UEdGraphNode_Comment* InNode);

private:

	void UpdateCommentNodeColor(const float InDeltaTime);
	void UpdateCommentNodeScale(const float InDeltaTime);
	void UpdateCommentNodeAngle(const float InDeltaTime);
	
	float ColorAnimationElapsedSeconds;
	float ScaleAnimationElapsedSeconds;
	float CurrentCommentNodeAngle;
};
