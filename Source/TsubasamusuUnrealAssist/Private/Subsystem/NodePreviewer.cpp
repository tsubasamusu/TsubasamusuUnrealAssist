// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "NodePreviewer.h"
#include "BlueprintActionMenuItem.h"
#include "BlueprintNodeSpawner.h"
#include "K2Node_ActorBoundEvent.h"
#include "K2Node_ComponentBoundEvent.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_Event.h"
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
	if (!LastNodeMenuWidget.IsValid())
	{
		if (IsValid(GhostBlueprint))
		{
			GhostBlueprint->MarkAsGarbage();
			GhostBlueprint = nullptr;
		}
	}
	
	const TSharedPtr<SWidget> HoveredWidget = GetHoveredWidget();
	
	if (!HoveredWidget.IsValid() || !IsDescendantOfBlueprintPaletteItem(HoveredWidget))
	{
		return;
	}
	
	const TSharedPtr<FGraphActionNode> GraphActionNode = GetGraphActionNode(HoveredWidget);

	if (!GraphActionNode.IsValid() || !IsNodeSpawnAction(GraphActionNode))
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

	if (LastCreatedNode.IsValid())
	{
		LastCreatedNode->DestroyNode();
	}
	
	const TSharedPtr<SWidget> CurrentNodeMenuWidget = FindParentWidget<SWidget>(HoveredWidget, TEXT("SBlueprintActionMenu"));
	
	if (!LastNodeMenuWidget.IsValid() || CurrentNodeMenuWidget != LastNodeMenuWidget.Pin())
	{
		RecreateGhost();
		LastNodeMenuWidget = CurrentNodeMenuWidget;
	}
	
	if (!IsValid(GhostBlueprint) || !GhostGraph.IsValid())
	{
		return;
	}
	
	LastCreatedNode = CreateOrFindNode(GraphActionNode);

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

	FVector2f NodeWidgetSize = NodeWidget->GetDesiredSize();
	TSharedRef<SWidget> FinalNodeWidget = NodeWidget.ToSharedRef();
	
	const UK2Node* K2Node = Cast<UK2Node>(LastCreatedNode);
	
	if (IsValid(K2Node))
	{
		const FName CornerIconName = K2Node->GetCornerIcon();
		
		if (!CornerIconName.IsNone())
		{
			const FSlateBrush* IconBrush = FAppStyle::GetBrush(CornerIconName);
			
			const FVector2f IconSize = IconBrush->GetImageSize();
			const FVector2f HalfIconSize = IconSize / 2.f;
			
			// Normally, setting "NodeWidgetSize.X" to "NodeWidgetSize.X + HalfIconSize.X" should be fine,
			// but for some reason, this sometimes causes the corner icon to be cut off,
			// so we increase "NodeWidgetSize.X" slightly.
			// Example: UWidgetAnimationPlayCallbackProxy::NewPlayAnimationTimeRangeProxyObject() (Play Animation Time Range with Finished event)
			NodeWidgetSize = FVector2f(NodeWidgetSize.X + IconSize.X, NodeWidgetSize.Y + HalfIconSize.Y);
 
			FinalNodeWidget = SNew(SBox)
			.WidthOverride(NodeWidgetSize.X)
			.HeightOverride(NodeWidgetSize.Y)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Left)
				[
					SNew(SBox)
					.HeightOverride(HalfIconSize.Y)
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Left)
				[
					SNew(SOverlay)
					+ SOverlay::Slot()
					[
						NodeWidget.ToSharedRef()
					]
					+ SOverlay::Slot()
					.HAlign(HAlign_Right)
					.VAlign(VAlign_Top)
					.Padding(FMargin(0, -HalfIconSize.X, -HalfIconSize.Y, 0)) 
					[
						SNew(SImage)
						.Image(IconBrush)
					]
				]
			];
		}
	}
	
	const TSharedRef<SToolTip> NewToolTip = SNew(SToolTip)
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Left)
		[
			SNew(SBox)
			.HeightOverride(NodeWidgetSize.Y * TsubasamusuUnrealAssistSettings->NodePreviewScale)
			[
				SNew(SScaleBox)
				.Stretch(EStretch::ScaleToFitY)
				[
					FinalNodeWidget
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

void UNodePreviewer::RecreateGhost()
{
	if (IsValid(GhostBlueprint))
	{
		GhostBlueprint->MarkAsGarbage();
		GhostBlueprint = nullptr;
	}
	
	const UEdGraph* FocusedGraph = GetFocusedGraph();
	
	if (IsValid(FocusedGraph))
	{
		const UBlueprint* FocusedBlueprint = FBlueprintEditorUtils::FindBlueprintForGraph(FocusedGraph);
		
		if (IsValid(FocusedBlueprint))
		{
			// Disable Compile
			FBlueprintDuplicationScopeFlags Scope(FBlueprintDuplicationScopeFlags::NoExtraCompilation);
			
			const FString GhostName = FString::Printf(TEXT("%s_%s"), *FocusedBlueprint->GetName(), *FGuid::NewGuid().ToString());
			GhostBlueprint = DuplicateObject<UBlueprint>(FocusedBlueprint, GetTransientPackage(), FName(*GhostName));

			TArray<UEdGraph*> GhostBlueprintGraphs;
			GhostBlueprint->GetAllGraphs(GhostBlueprintGraphs);

			UEdGraph** FoundGraph = GhostBlueprintGraphs.FindByPredicate([FocusedGraph](const UEdGraph* InGhostBlueprintGraph)
			{
				return InGhostBlueprintGraph->GetFName() == FocusedGraph->GetFName();
			});
			
			if (FoundGraph)
			{
				GhostGraph = *FoundGraph;
			}
		}
	}
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

UEdGraph* UNodePreviewer::GetGraphOfType(const EGraphType InGraphType, const UBlueprint* InBlueprint)
{
	check(IsValid(InBlueprint));
	
	TArray<UEdGraph*> AllGraphs;
	InBlueprint->GetAllGraphs(AllGraphs);
	
	UEdGraph** FoundGraph = AllGraphs.FindByPredicate([InGraphType](const UEdGraph* InGraph)
	{
		return IsValid(InGraph) && GetGraphType(InGraph) == InGraphType;
	});
	
	if (FoundGraph)
	{
		return *FoundGraph;
	}
	
	return nullptr;
}

EGraphType UNodePreviewer::GetGraphType(const UEdGraph* InGraph)
{
	check(IsValid(InGraph));
	UEdGraph* Graph = const_cast<UEdGraph*>(InGraph);
	
	for (UObject* Outer = Graph; Outer; Outer = Outer->GetOuter())
	{
		const UBlueprint* Blueprint = Cast<UBlueprint>(Outer);
		
		if (IsValid(Blueprint))
		{
			if (Blueprint->BlueprintType == BPTYPE_MacroLibrary || Blueprint->MacroGraphs.Contains(Graph))
			{
				return GT_Macro;
			}
			
			if (Blueprint->UbergraphPages.Contains(Graph))
			{
				return GT_Ubergraph;
			}
			
			if (Blueprint->BlueprintType == BPTYPE_FunctionLibrary || Blueprint->FunctionGraphs.Contains(Graph))
			{
				return GT_Function; 
			}
		}
		else
		{
			Graph = Cast<UEdGraph>(Outer);
		}
	}
	
	return GT_Function;
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

UEdGraphNode* UNodePreviewer::CreateOrFindNode(const TSharedPtr<FGraphActionNode> InGraphActionNode) const
{
	if (InGraphActionNode.IsValid() && InGraphActionNode->HasValidAction() && IsValid(GhostBlueprint) && GhostGraph.IsValid())
	{
		const TSharedPtr<FEdGraphSchemaAction> PrimaryAction = InGraphActionNode->GetPrimaryAction();
		
		const TSharedPtr<FBlueprintActionMenuItem> BlueprintActionMenuItem = StaticCastSharedPtr<FBlueprintActionMenuItem>(PrimaryAction);
		const UBlueprintNodeSpawner* BlueprintNodeSpawner = BlueprintActionMenuItem->GetRawAction();
		
		const UEdGraphNode* TemplateNode = BlueprintNodeSpawner->GetTemplateNode();
		const UK2Node_Event* TemplateEventNode = Cast<UK2Node_Event>(TemplateNode);
	
		if(IsValid(TemplateEventNode))
		{
			if(IsValid(GhostBlueprint->SkeletonGeneratedClass))
			{
				const UK2Node_ActorBoundEvent* TemplateActorBoundEventNode = Cast<UK2Node_ActorBoundEvent>(TemplateEventNode);
				const UK2Node_ComponentBoundEvent* TemplateComponentBoundEvent = Cast<UK2Node_ComponentBoundEvent>(TemplateEventNode);
				
				const bool bIsCustomEventNode = TemplateEventNode->IsA(UK2Node_CustomEvent::StaticClass());
				const UEdGraphNode* ExistingNode = nullptr;
				
				if (IsValid(TemplateActorBoundEventNode))
				{
					ExistingNode = FKismetEditorUtilities::FindBoundEventForActor(TemplateActorBoundEventNode->GetReferencedLevelActor(), TemplateActorBoundEventNode->DelegatePropertyName);
				}
				else if (IsValid(TemplateComponentBoundEvent))
				{
					ExistingNode = FKismetEditorUtilities::FindBoundEventForComponent(GhostBlueprint, TemplateComponentBoundEvent->DelegatePropertyName, TemplateComponentBoundEvent->ComponentPropertyName);
				}
				else if (bIsCustomEventNode)
				{
					ExistingNode = FBlueprintEditorUtils::FindCustomEventNode(GhostBlueprint, TemplateEventNode->CustomFunctionName);
				}
				else
				{
					const UFunction* EventSignatureFunction =
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 4, 0)
						TemplateEventNode->FindEventSignatureFunction();
#else
						const_cast<UK2Node_Event*>(TemplateEventNode)->FindEventSignatureFunction();
#endif
					
					if (IsValid(EventSignatureFunction))
					{
						UClass* OwnerClass = EventSignatureFunction->GetOwnerClass();
						ExistingNode = FBlueprintEditorUtils::FindOverrideForFunction(GhostBlueprint, OwnerClass->GetAuthoritativeClass(), EventSignatureFunction->GetFName());
					}
				}
	
				if (IsValid(ExistingNode))
				{
					return const_cast<UEdGraphNode*>(ExistingNode);
				}
			}
			
			if (GetGraphType(GhostGraph.Get()) != GT_Ubergraph)
			{
				UEdGraph* Ubergraph = GetGraphOfType(GT_Ubergraph, GhostBlueprint);
				
				if (IsValid(Ubergraph))
				{
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 6, 0)
					return PrimaryAction->PerformAction(Ubergraph, nullptr, FVector2f());
#else
					return PrimaryAction->PerformAction(Ubergraph, nullptr, FVector2D());
#endif
				}
			}
		}
		
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 6, 0)
		return PrimaryAction->PerformAction(GhostGraph.Get(), nullptr, FVector2f());
#else
		return PrimaryAction->PerformAction(GhostGraph.Get(), nullptr, FVector2D());
#endif
	}
	
	return nullptr;
}

bool UNodePreviewer::IsNodeSpawnAction(const TSharedPtr<FGraphActionNode> InGraphActionNode)
{
	if (InGraphActionNode.IsValid() && InGraphActionNode->HasValidAction())
	{
		const TSharedPtr<FEdGraphSchemaAction> PrimaryAction = InGraphActionNode->GetPrimaryAction();
	
		if (PrimaryAction.IsValid() && PrimaryAction->IsA(FBlueprintActionMenuItem::StaticGetTypeId()))
		{
			const TSharedPtr<FBlueprintActionMenuItem> BlueprintActionMenuItem = StaticCastSharedPtr<FBlueprintActionMenuItem>(PrimaryAction);
			const UBlueprintNodeSpawner* BlueprintNodeSpawner = BlueprintActionMenuItem->GetRawAction();
			
			return IsValid(BlueprintNodeSpawner);
		}
	}
	
	return false;
}

bool UNodePreviewer::IsDescendantOfBlueprintPaletteItem(const TSharedPtr<SWidget> InWidget)
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
