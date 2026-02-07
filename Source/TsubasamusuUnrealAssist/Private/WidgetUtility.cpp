// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "WidgetUtility.h"

TSharedPtr<SWidget> FWidgetUtility::GetHoveredWidget()
{
	FSlateApplication& SlateApplication = FSlateApplication::Get();
	const FVector2f CursorPosition = SlateApplication.GetCursorPos();
	FWidgetPath WidgetPath = SlateApplication.LocateWindowUnderMouse(CursorPosition, SlateApplication.GetInteractiveTopLevelWindows());
 
	if (WidgetPath.IsValid())
	{
		TSharedPtr<SWidget> HoveredWidget = WidgetPath.Widgets.Last().Widget;
		
		if (HoveredWidget.IsValid())
		{
			return HoveredWidget;
		}
	}
	
	return nullptr;
}
