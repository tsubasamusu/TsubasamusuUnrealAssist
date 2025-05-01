// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphNodeCustomizableComment.h"

namespace GamingCommentNodeDefinitions
{
	static constexpr float GamingAnimationDuration = 1.0f;

	static constexpr float ScalingAnimationDuration = 1.0f;

	static constexpr float RotatingAnglePerSeconds = 720.0f;

	static constexpr float MaxCommentNodeScale = 1.5f;
}

class TSUBASAMUSUUNREALASSIST_API SGraphNodeGamingComment : public SGraphNodeCustomizableComment
{
	typedef SGraphNodeGamingComment Super;

public:
	
	SLATE_BEGIN_ARGS(SGraphNodeGamingComment) {}
	SLATE_END_ARGS()
	
	//~ Begin SWidget Interface
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	//~ End SWidget Interface
	
	void Construct(const FArguments& InArgs, UEdGraphNode_Comment* InNode);

protected:

	//~ Begin SGraphNodeCustomizableComment Interface
	virtual FSlateColor GetCommentBodyColor() const override;
	virtual FSlateColor GetTitleBarColor() const override;
	virtual FSlateColor GetCommentBubbleColor() const override;
	//~ End SGraphNodeCustomizableComment Interface
	
private:

	void UpdateGamingAnimationColor(const float InDeltaTime);
	void UpdateCommentNodeScale(const float InDeltaTime);
	void UpdateCommentNodeAngle(const float InDeltaTime);
	
	FLinearColor GetGamingAnimationColor() const;
	
	float GamingAnimationElapsedSeconds;
	float ScalingAnimationElapsedSeconds;
	float CurrentCommentNodeAngle;
};
