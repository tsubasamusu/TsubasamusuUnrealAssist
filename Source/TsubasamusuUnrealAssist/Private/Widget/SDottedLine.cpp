// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "Widget/SDottedLine.h"

FVector2D SDottedLine::ComputeDesiredSize(float) const
{
	return Orientation == Orient_Vertical ? FVector2D(Thickness, 1.0f) : FVector2D(1.0f, Thickness);
}

int32 SDottedLine::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	constexpr float Dotted = 8.0f;
	constexpr float InDottedRatio = 0.5f;
	constexpr float Visible = Dotted * InDottedRatio;

	const FVector2f Center(AllottedGeometry.GetLocalSize().X * 0.5f, 0.0f);
	float PointY = 0.0f;

	while (PointY < AllottedGeometry.GetLocalSize().Y)
	{
		TArray<FVector2f> Points;
		
		Points.Add(FVector2f(Center.X, PointY));
		Points.Add(FVector2f(Center.X, FMath::Min(PointY + Visible, AllottedGeometry.GetLocalSize().Y)));

		FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), MoveTemp(Points), ESlateDrawEffect::None, Color.Get().GetSpecifiedColor(), true, 3.0f);

		PointY += Dotted;
	}
	
	return LayerId + 1;
}
