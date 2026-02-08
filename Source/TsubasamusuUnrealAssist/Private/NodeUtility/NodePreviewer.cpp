// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "NodeUtility/NodePreviewer.h"
#include "BlueprintActionMenuItem.h"
#include "BlueprintNodeSpawner.h"
#include "GraphActionNode.h"
#include "NodeFactory.h"
#include "SGraphNode.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

void FNodePreviewer::TryPreviewNode()
{
	const TSharedPtr<SWidget> HoveredWidget = GetHoveredWidget();
	
	if (!HoveredWidget.IsValid())
	{
		return;
	}
	
	if (!IsNodeSelectionWidget(HoveredWidget))
	{
		return;
	}
	
	const TSharedPtr<FGraphActionNode> GraphActionNode = GetGraphActionNodeFromWidget(HoveredWidget);
	check(GraphActionNode.IsValid());
	
	if (!CachedGraphActionNode.IsValid() || !CachedNodeWidget.IsValid() || GraphActionNode != CachedGraphActionNode)
	{
		UEdGraphNode* Node = CreateNodeFromGraphActionNode(GraphActionNode);
		
		if (IsValid(Node))
		{
			TSharedPtr<SGraphNode> NodeWidget = CreateNodeWidget(Node);
			check(NodeWidget.IsValid());
			
			CachedGraphActionNode.Reset();
			CachedNodeWidget.Reset();
			
			CachedGraphActionNode = GraphActionNode;
			CachedNodeWidget = NodeWidget;
		}
	}
	
	if (!CachedNodeWidget.IsValid())
	{
		return;
	}
}

void FNodePreviewer::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(CachedBlueprint);
	Collector.AddReferencedObject(CachedGraph);
}

TSharedPtr<SWidget> FNodePreviewer::GetHoveredWidget()
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

bool FNodePreviewer::IsNodeSelectionWidget(const TSharedPtr<SWidget> InWidget)
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

TSharedPtr<FGraphActionNode> FNodePreviewer::GetGraphActionNodeFromWidget(const TSharedPtr<SWidget> InWidget)
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

TSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>> FNodePreviewer::GetNodeTreeViewFromWidget(const TSharedPtr<SWidget> InWidget)
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

TSharedPtr<SGraphNode> FNodePreviewer::CreateNodeWidget(UEdGraphNode* InNode)
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

UEdGraphNode* FNodePreviewer::CreateNodeFromGraphActionNode(const TSharedPtr<FGraphActionNode> InGraphActionNode)
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
	
	if (!IsValid(CachedBlueprint))
	{
		CachedBlueprint = FKismetEditorUtilities::CreateBlueprint(UObject::StaticClass(),GetTransientPackage(), NAME_None,BPTYPE_Normal,UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
	}
	
	if (!IsValid(CachedGraph))
	{
		CachedGraph= FBlueprintEditorUtils::CreateNewGraph(CachedBlueprint, FName("TempGraph"),UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
	}
	
	return BlueprintNodeSpawner->Invoke(CachedGraph, IBlueprintNodeBinder::FBindingSet(), FVector2D());
}

TSharedPtr<SToolTip> FNodePreviewer::FindChildToolTipWidget(const TSharedPtr<SWidget> InWidget)
{
	if (!InWidget.IsValid())
	{
		return nullptr;
	}
	
	if (InWidget->GetType() == TEXT("SToolTip"))
	{
		return StaticCastSharedPtr<SToolTip>(InWidget);
	}
	
	for (int32 ChildIndex = 0; ChildIndex < InWidget->GetChildren()->Num(); ChildIndex++)
	{
		TSharedRef<SWidget> ChildWidget = InWidget->GetChildren()->GetChildAt(ChildIndex);
		TSharedPtr<SToolTip> ToolTipWidget = FindChildToolTipWidget(ChildWidget.ToSharedPtr());
		
		if (ToolTipWidget.IsValid())
		{
			return ToolTipWidget;
		}
	}
	
	return nullptr;
}
