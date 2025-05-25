// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SLeafWidget.h"

class SDottedLine final : public SLeafWidget
{
public:
	
	SLATE_BEGIN_ARGS(SDottedLine)
		: _Thickness(3.0f)
		, _OneCycleLength(8.0f)
		, _DottedRatio(0.5f)
		, _Color(FSlateColor::UseForeground())
		, _Orientation(Orient_Vertical)
	{}
		
	SLATE_ARGUMENT(float, Thickness)
	SLATE_ARGUMENT(float, OneCycleLength)
	SLATE_ARGUMENT(float, DottedRatio)
	SLATE_ATTRIBUTE(FSlateColor, Color)
	SLATE_ARGUMENT(EOrientation, Orientation)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs)
	{
		Thickness = InArgs._Thickness;
		OneCycleLength = InArgs._OneCycleLength;
		DottedRatio = InArgs._DottedRatio;
		Color = InArgs._Color;
		Orientation = InArgs._Orientation;
	}

	//~ Begin SWidget Interface
	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	//~ End SWidget Interface
	
private:
	
	float Thickness;
	float OneCycleLength;
	float DottedRatio;
	
	TAttribute<FSlateColor> Color;
	
	EOrientation Orientation;
};
