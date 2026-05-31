// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "NodePreviewer.h"
#include "BlueprintActionMenuItem.h"
#include "BlueprintNodeSpawner.h"
#include "NodeFactory.h"
#include "SGraphNode.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Setting/TsubasamusuUnrealAssistSettings.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Type/TsubasamusuUnrealAssistMacros.h"

#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 3, 0)
#include "GraphActionNode.h"
#else
#include "GraphEditor/Private/GraphActionNode.h"
#endif

template<typename WidgetClass>
static TSharedPtr<WidgetClass> FindParentWidget(const TSharedPtr<SWidget> InWidget, const FName& InWidgetTypeToFind)
{
	static_assert(TIsDerivedFrom<WidgetClass, SWidget>::Value, "WidgetClass must be derived from SWidget.");

	for (TSharedPtr<SWidget> Widget = InWidget; Widget.IsValid(); Widget = Widget->GetParentWidget())
	{
		if (Widget->GetType() == InWidgetTypeToFind)
		{
			return StaticCastSharedPtr<WidgetClass>(Widget);
		}
	}

	return nullptr;
}

template<typename WidgetClass>
static TSharedPtr<WidgetClass> FindChildWidget(const TSharedPtr<SWidget> InWidget, const FName& InWidgetTypeToFind)
{
	static_assert(TIsDerivedFrom<WidgetClass, SWidget>::Value, "WidgetClass must be derived from SWidget.");
	
	if (InWidget.IsValid())
	{
		if (InWidget->GetType() == InWidgetTypeToFind)
		{
			return StaticCastSharedPtr<WidgetClass>(InWidget);
		}

		const FChildren* Children = InWidget->GetChildren();

		if (Children)
		{
			for (int32 i = 0; i < Children->Num(); ++i)
			{
				TSharedRef<SWidget> ChildWidget = ConstCastSharedRef<SWidget>(Children->GetChildAt(i));
				TSharedPtr<WidgetClass> FoundWidget = FindChildWidget<WidgetClass>(ChildWidget, InWidgetTypeToFind);

				if (FoundWidget.IsValid())
				{
					return FoundWidget;
				}
			}
		}
	}

	return nullptr;
}

void UNodePreviewer::Initialize(FSubsystemCollectionBase& InSubsystemCollectionBase)
{
	Super::Initialize(InSubsystemCollectionBase);
	
	GhostBlueprint = FKismetEditorUtilities::CreateBlueprint(UObject::StaticClass(), GetTransientPackage(), NAME_None, BPTYPE_Normal,UAnimBlueprint::StaticClass(), UAnimBlueprintGeneratedClass::StaticClass());
	GhostGraph = FBlueprintEditorUtils::CreateNewGraph(GhostBlueprint, TEXT("NodePreviewGraph"), UAnimationGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
}

void UNodePreviewer::Tick(const float InDeltaTime)
{
	const TWeakObjectPtr<UTsubasamusuUnrealAssistSettings> TsubasamusuUnrealAssistSettings = GetCachedTsubasamusuUnrealAssistSettings();
	
	if (TsubasamusuUnrealAssistSettings.IsValid() && TsubasamusuUnrealAssistSettings->bEnableNodePreview)
	{
		TryPreviewNode();
	}
}

void UNodePreviewer::TryPreviewNode()
{
	const TSharedPtr<SWidget> HoveredWidget = GetHoveredWidget();
	
	if (!HoveredWidget.IsValid() || !IsNodeSelectionWidget(HoveredWidget))
	{
		return;
	}
	
	const TSharedPtr<SWidget> ToolTipDisplayer = FindToolTipDisplayer(HoveredWidget);

	if (!ToolTipDisplayer.IsValid())
	{
		return;
	}
	
	const TSharedPtr<SToolTip> CurrentToolTipWidget = StaticCastSharedPtr<SToolTip>(ToolTipDisplayer->GetToolTip());

	// If the widget displaying tool tip is the same as the previous one, do nothing.
	if (ToolTipEditedWidget.IsValid() && ToolTipDisplayer == ToolTipEditedWidget.Pin())
	{
		return;
	}
	
	// To preview a different node than the last one, restore the tool tip from the previous edit.
	if (ToolTipEditedWidget.IsValid() && EditedToolTipWidget.IsValid())
	{
		ToolTipEditedWidget.Pin()->SetToolTip(EditedToolTipWidget.Pin());

		ToolTipEditedWidget.Reset();
		EditedToolTipWidget.Reset();
	}

	const TSharedPtr<FGraphActionNode> GraphActionNode = GetGraphActionNode(HoveredWidget);

	if (!GraphActionNode.IsValid())
	{
		return;
	}
	if (LastCreatedNode.IsValid())
	{
		LastCreatedNode->DestroyNode();
	}
	
	LastCreatedNode = CreateNode(GraphActionNode);

	if (!LastCreatedNode.IsValid())
	{
		return;
	}

	const TWeakObjectPtr<UTsubasamusuUnrealAssistSettings> TsubasamusuUnrealAssistSettings = GetCachedTsubasamusuUnrealAssistSettings();
	
	if (TsubasamusuUnrealAssistSettings.IsValid() && TsubasamusuUnrealAssistSettings->bAlsoPreviewAdvancedView && LastCreatedNode->AdvancedPinDisplay != ENodeAdvancedPins::NoPins)
	{
		LastCreatedNode->AdvancedPinDisplay = ENodeAdvancedPins::Shown;
	}

	const TSharedPtr<SGraphNode> NodeWidget = CreateNodeWidget(LastCreatedNode.Get());

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
		.Padding(0.f, 5.f, 0.f, 0.f)
		[
			CurrentToolTipWidget.ToSharedRef()
		]
	];

	ToolTipDisplayer->SetToolTip(NewToolTip);

	ToolTipEditedWidget = ToolTipDisplayer;
	EditedToolTipWidget = CurrentToolTipWidget;
}

TSharedPtr<SWidget> UNodePreviewer::GetHoveredWidget()
{
	FSlateApplication& SlateApplication = FSlateApplication::Get();
	
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 2, 0)
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

TSharedPtr<FGraphActionNode> UNodePreviewer::GetGraphActionNode(const TSharedPtr<SWidget> InWidget)
{
	const TSharedPtr<STableRow<TSharedPtr<FGraphActionNode>>> TableRow = FindParentWidget<STableRow<TSharedPtr<FGraphActionNode>>>(InWidget, TEXT("STableRow< TSharedPtr<FGraphActionNode> >"));
	
	if (TableRow.IsValid())
	{
		const TSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>> NodeTreeView= FindParentWidget<STreeView<TSharedPtr<FGraphActionNode>>>(TableRow, TEXT("STreeView< TSharedPtr<FGraphActionNode> >"));
	
		if (NodeTreeView.IsValid())
		{
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 4, 0)
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

TSharedPtr<SGraphNode> UNodePreviewer::CreateNodeWidget(UEdGraphNode* InNode)
{
	if (IsValid(InNode))
	{
		InNode->AllocateDefaultPins();
		InNode->ReconstructNode();
	
		TSharedPtr<SGraphNode> NodeWidget = FNodeFactory::CreateNodeWidget(InNode);
	
		NodeWidget->UpdateGraphNode();
		NodeWidget->SlatePrepass();
	
		return NodeWidget;
	}
	
	return nullptr;
}

UEdGraphNode* UNodePreviewer::CreateNode(const TSharedPtr<FGraphActionNode> InGraphActionNode) const
{
	if (InGraphActionNode.IsValid() && IsValid(GhostBlueprint) && IsValid(GhostGraph))
	{
		const TSharedPtr<FEdGraphSchemaAction> PrimaryAction = InGraphActionNode->GetPrimaryAction();
	
		if (PrimaryAction.IsValid() && PrimaryAction->IsA(FBlueprintActionMenuItem::StaticGetTypeId()))
		{
			const TSharedPtr<FBlueprintActionMenuItem> BlueprintActionMenuItem = StaticCastSharedPtr<FBlueprintActionMenuItem>(PrimaryAction);
			const UBlueprintNodeSpawner* BlueprintNodeSpawner = BlueprintActionMenuItem->GetRawAction();
			
			if (IsValid(BlueprintNodeSpawner))
			{
				return BlueprintNodeSpawner->Invoke(GhostGraph, IBlueprintNodeBinder::FBindingSet(), FVector2D());
			}
		}
	}
	
	return nullptr;
}

bool UNodePreviewer::IsNodeSelectionWidget(const TSharedPtr<SWidget> InWidget)
{
	const TSharedPtr<SWidget> BlueprintPaletteItem = FindParentWidget<SWidget>(InWidget, TEXT("SBlueprintPaletteItem"));
	return BlueprintPaletteItem.IsValid();
}

TSharedPtr<SWidget> UNodePreviewer::FindToolTipDisplayer(const TSharedPtr<SWidget> InNodeSelectionWidget)
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

UEdGraph* UNodePreviewer::GetFocusedGraph()
{
	const TSharedPtr<SDockTab> ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();

	if (ActiveTab.IsValid())
	{
		const TSharedRef<SWidget> ContentWidget = ActiveTab->GetContent();
		const TSharedPtr<SGraphEditor> GraphEditor = FindChildWidget<SGraphEditor>(ContentWidget, TEXT("SGraphEditor"));

		if (GraphEditor.IsValid())
		{
			return GraphEditor->GetCurrentGraph();
		}
	}
	
	return nullptr;
}
