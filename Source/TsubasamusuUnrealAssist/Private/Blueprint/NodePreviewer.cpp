// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "Blueprint/NodePreviewer.h"
#include "BlueprintActionMenuItem.h"
#include "BlueprintNodeSpawner.h"
#include "NodeFactory.h"
#include "SGraphNode.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Setting/EditorSettingsUtility.h"
#include "Setting/TsubasamusuUnrealAssistSettings.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3)
#include "GraphActionNode.h"
#else
#include "GraphEditor/Private/GraphActionNode.h"
#endif

void FNodePreviewer::TryPreviewNode()
{
	const TSharedPtr<SWidget> HoveredWidget = GetHoveredWidget();
	
	if (!HoveredWidget.IsValid() || !IsNodeSelectionWidget(HoveredWidget))
	{
		return;
	}
	
	const TSharedPtr<SWidget> WidgetDisplayingToolTip = FindWidgetDisplayingToolTipFromNodeSelectionWidget(HoveredWidget);
	
	if (!WidgetDisplayingToolTip.IsValid())
	{
		return;
	}
	
	const TSharedPtr<SToolTip> CurrentToolTipWidget = StaticCastSharedPtr<SToolTip>(WidgetDisplayingToolTip->GetToolTip());
	
	if (WidgetWhoseToolTipWasPreviouslyEdited.IsValid() && WidgetDisplayingToolTip == WidgetWhoseToolTipWasPreviouslyEdited.Pin())
	{
		return;
	}
	
	// To preview a different node than the last one, restore the tooltip from the previous edit.
	if (LastBeforeEditingToolTipWidget.IsValid())
	{
		WidgetWhoseToolTipWasPreviouslyEdited.Pin()->SetToolTip(LastBeforeEditingToolTipWidget.Pin());
		
		WidgetWhoseToolTipWasPreviouslyEdited.Reset();
		LastBeforeEditingToolTipWidget.Reset();
	}
	
	const TSharedPtr<FGraphActionNode> GraphActionNode = GetGraphActionNodeFromWidget(HoveredWidget);
	
	if (!GraphActionNode.IsValid())
	{
		return;
	}
	
	UEdGraphNode* Node = CreateNodeFromGraphActionNode(GraphActionNode);
	
	if (!IsValid(Node))
	{
		return;
	}
	
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
	
	if (TsubasamusuUnrealAssistSettings->bAlsoPreviewAdvancedView && Node->AdvancedPinDisplay != ENodeAdvancedPins::NoPins)
	{
		Node->AdvancedPinDisplay = ENodeAdvancedPins::Shown;
	}
	
	const TSharedPtr<SGraphNode> NodeWidget = CreateNodeWidget(Node);
	
	if (!NodeWidget.IsValid())
	{
		return;
	}
	
	const TSharedRef<SToolTip> NewToolTip = SNew(SToolTip)
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Left)
		[
			SNew(SBox)
			.HeightOverride(NodeWidget->GetDesiredSize().Y * TsubasamusuUnrealAssistSettings->NodePreviewScale)
			[
				SNew(SScaleBox)
				.Stretch(EStretch::ScaleToFitY)
				[
					NodeWidget.ToSharedRef()
				]
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(FMargin(0.f, 5.f, 0.f, 0.f))
		[
			CurrentToolTipWidget.ToSharedRef()
		]
	];
	
	WidgetDisplayingToolTip->SetToolTip(NewToolTip);
	
	WidgetWhoseToolTipWasPreviouslyEdited = WidgetDisplayingToolTip;
	LastBeforeEditingToolTipWidget = CurrentToolTipWidget;
}

void FNodePreviewer::AddReferencedObjects(FReferenceCollector& InReferenceCollector)
{
	InReferenceCollector.AddReferencedObject(CachedTemporaryBlueprint);
	InReferenceCollector.AddReferencedObject(CachedTemporaryGraph);
}

TSharedPtr<SWidget> FNodePreviewer::GetHoveredWidget()
{
	FSlateApplication& SlateApplication = FSlateApplication::Get();
	
#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 2)
	const FVector2f CursorPosition = SlateApplication.GetCursorPos();
#else
	const FVector2D CursorPosition = SlateApplication.GetCursorPos();
#endif
	
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

TSharedPtr<FGraphActionNode> FNodePreviewer::GetGraphActionNodeFromWidget(const TSharedPtr<SWidget> InWidget)
{
	const TSharedPtr<STableRow<TSharedPtr<FGraphActionNode>>> TableRow = FindParentWidget<STableRow<TSharedPtr<FGraphActionNode>>>(InWidget, TEXT("STableRow< TSharedPtr<FGraphActionNode> >"));
	
	if (TableRow.IsValid())
	{
		const TSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>> NodeTreeView= FindParentWidget<STreeView<TSharedPtr<FGraphActionNode>>>(TableRow, TEXT("STreeView< TSharedPtr<FGraphActionNode> >"));
	
		if (NodeTreeView.IsValid())
		{
#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 4)
			const TObjectPtrWrapTypeOf<TSharedPtr<FGraphActionNode>>* GraphActionNodePtr = NodeTreeView->Private_ItemFromWidget(TableRow.Get());
			
			if (GraphActionNodePtr->IsValid())
			{
				return GraphActionNodePtr->ToSharedRef();
			}
#else
			const TSharedPtr<FGraphActionNode>* GraphActionNodePtr = NodeTreeView->Private_ItemFromWidget(TableRow.Get());
			
			if (GraphActionNodePtr && GraphActionNodePtr->IsValid())
			{
				return *GraphActionNodePtr;
			}
#endif
		}
	}
	
	return nullptr;
}

bool FNodePreviewer::IsNodeSelectionWidget(const TSharedPtr<SWidget> InWidget)
{
	const TSharedPtr<SWidget> SBlueprintPaletteItem = FindParentWidget<SWidget>(InWidget, TEXT("SBlueprintPaletteItem"));
	return SBlueprintPaletteItem.IsValid();
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
	
	if (!IsValid(CachedTemporaryBlueprint))
	{
		CachedTemporaryBlueprint = FKismetEditorUtilities::CreateBlueprint(UObject::StaticClass(), GetTransientPackage(), NAME_None, BPTYPE_Normal,UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
	}
	
	if (!IsValid(CachedTemporaryGraph))
	{
		CachedTemporaryGraph = FBlueprintEditorUtils::CreateNewGraph(CachedTemporaryBlueprint, TEXT("NodePreviewGraph"), UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
	}
	
	return BlueprintNodeSpawner->Invoke(CachedTemporaryGraph, IBlueprintNodeBinder::FBindingSet(), FVector2D());
}

TSharedPtr<SWidget> FNodePreviewer::FindWidgetDisplayingToolTipFromNodeSelectionWidget(const TSharedPtr<SWidget> InNodeSelectionWidget)
{
	if (InNodeSelectionWidget.IsValid())
	{
		if (InNodeSelectionWidget->GetType() == TEXT("SLayeredImage"))
		{
			return InNodeSelectionWidget;
		}
	
		TSharedPtr<SInlineEditableTextBlock> InlineEditableTextBlock = FindParentWidget<SInlineEditableTextBlock>(InNodeSelectionWidget, TEXT("SInlineEditableTextBlock"));
	
		if (InlineEditableTextBlock.IsValid())
		{
			return InlineEditableTextBlock;
		}
	}
	
	return nullptr;
}
