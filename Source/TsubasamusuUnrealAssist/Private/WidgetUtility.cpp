// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "WidgetUtility.h"
#include "GraphActionNode.h"

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

bool FWidgetUtility::IsNodeSelectionWidget(const TSharedPtr<SWidget> InWidget)
{
	TSharedPtr<SWidget> TargetWidget = InWidget;
	
	while (true)
	{
		if (!TargetWidget.IsValid())
		{
			break;
		}
		
		if (TargetWidget->GetType() == TEXT("SBlueprintPaletteItem"))
		{
			return true;
		}
		
		TargetWidget = TargetWidget->GetParentWidget();
	}
	
	return false;
}

TSharedPtr<FGraphActionNode> FWidgetUtility::GetGraphActionNodeFromWidget(const TSharedPtr<SWidget> InWidget)
{
	TSharedPtr<SWidget> TargetWidget = InWidget;
	
	while (true)
	{
		if (!TargetWidget.IsValid())
		{
			break;
		}
		
		if (TargetWidget->GetType() == TEXT("STableRow< TSharedPtr<FGraphActionNode> >"))
		{
			TSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>> NodeTreeView= GetNodeTreeViewFromWidget(TargetWidget);
			TSharedPtr<STableRow<TSharedPtr<FGraphActionNode>>> TableRow = StaticCastSharedPtr<STableRow<TSharedPtr<FGraphActionNode>>>(TargetWidget);
			
			if (NodeTreeView.IsValid())
			{
				const TObjectPtrWrapTypeOf<TSharedPtr<FGraphActionNode>>* GraphActionNodePtr = NodeTreeView->Private_ItemFromWidget(TableRow.Get());
				
				if (GraphActionNodePtr->IsValid())
				{
					return GraphActionNodePtr->ToSharedRef();
				}
			}
		}
		
		TargetWidget = TargetWidget->GetParentWidget();
	}
	
	return nullptr;
}

TSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>> FWidgetUtility::GetNodeTreeViewFromWidget(const TSharedPtr<SWidget> InWidget)
{
	TSharedPtr<SWidget> TargetWidget = InWidget;
	
	while (true)
	{
		if (!TargetWidget.IsValid())
		{
			break;
		}
		
		if (TargetWidget->GetType() == TEXT("STreeView< TSharedPtr<FGraphActionNode> >"))
		{
			return StaticCastSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>>(TargetWidget);
		}
		
		TargetWidget = TargetWidget->GetParentWidget();
	}
	
	return nullptr;
}
