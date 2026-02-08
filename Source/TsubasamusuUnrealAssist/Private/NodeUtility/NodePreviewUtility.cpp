// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "NodeUtility/NodePreviewUtility.h"
#include "BlueprintActionMenuItem.h"
#include "BlueprintNodeSpawner.h"
#include "GraphActionNode.h"
#include "NodeFactory.h"
#include "SGraphNode.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

TSharedPtr<SWidget> FNodePreviewUtility::GetHoveredWidget()
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

bool FNodePreviewUtility::IsNodeSelectionWidget(const TSharedPtr<SWidget> InWidget)
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

TSharedPtr<FGraphActionNode> FNodePreviewUtility::GetGraphActionNodeFromWidget(const TSharedPtr<SWidget> InWidget)
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

TSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>> FNodePreviewUtility::GetNodeTreeViewFromWidget(const TSharedPtr<SWidget> InWidget)
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

UEdGraphNode* FNodePreviewUtility::CreateNodeFromGraphActionNode(const TSharedPtr<FGraphActionNode> InGraphActionNode)
{
	if (!InGraphActionNode.IsValid())
	{
		return nullptr;
	}
	
	TSharedPtr<FEdGraphSchemaAction> PrimaryAction = InGraphActionNode->GetPrimaryAction();
	
	if (!PrimaryAction.IsValid() || !PrimaryAction->IsA(FBlueprintActionMenuItem::StaticGetTypeId()))
	{
		return nullptr;
	}

	const TSharedPtr<FBlueprintActionMenuItem> BlueprintActionMenuItem = StaticCastSharedPtr<FBlueprintActionMenuItem>(PrimaryAction);
	const UBlueprintNodeSpawner* BlueprintNodeSpawner = BlueprintActionMenuItem->GetRawAction();
					
	if (!IsValid(BlueprintNodeSpawner))
	{
		return nullptr;
	}
	
	UBlueprint* TemporaryBlueprint = FKismetEditorUtilities::CreateBlueprint(UObject::StaticClass(),GetTransientPackage(), NAME_None,BPTYPE_Normal,UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
	UEdGraph* TemporaryGraph = FBlueprintEditorUtils::CreateNewGraph(TemporaryBlueprint, FName("TempGraph"),UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
	
	return  BlueprintNodeSpawner->Invoke(TemporaryGraph, IBlueprintNodeBinder::FBindingSet(), FVector2D());
}

TSharedPtr<SGraphNode> FNodePreviewUtility::CreateNodeWidget(UEdGraphNode* InNode)
{
	if (!IsValid(InNode))
	{
		return nullptr;
	}
	
	InNode->AllocateDefaultPins();
	InNode->ReconstructNode();
	
	TSharedPtr<SGraphNode> NodeWidget = FNodeFactory::CreateNodeWidget(InNode);
	NodeWidget->UpdateGraphNode();
	NodeWidget->SlatePrepass();
	
	return NodeWidget;
}
