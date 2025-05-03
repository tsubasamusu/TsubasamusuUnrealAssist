// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "SGraphNodeCustomizableComment.h"
#include "EdGraphNode_Comment.h"
#include "SCommentBubble.h"
#include "SGraphPanel.h"
#include "TutorialMetaData.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

bool SGraphNodeCustomizableComment::MouseIsInSelectionZone() const
{
	return IsInSelectionZone(MouseLocatedZone);
}

bool SGraphNodeCustomizableComment::IsInSelectionZone(const ECommentNodeZone InCommentNodeZone)
{
	return InCommentNodeZone == RightBorder || InCommentNodeZone == BottomBorder || InCommentNodeZone == BottomRightBorder || InCommentNodeZone == LeftBorder || InCommentNodeZone == TopBorder || InCommentNodeZone == TopLeftBorder || InCommentNodeZone == TopRightBorder || InCommentNodeZone == BottomLeftBorder;
}

void SGraphNodeCustomizableComment::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if(!bUserIsDragging)
	{
		MouseLocatedZone = GetMouseZone(MyGeometry, MouseEvent);
		
		SNode::OnMouseEnter(MyGeometry, MouseEvent);
	}
}

void SGraphNodeCustomizableComment::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	if(!bUserIsDragging)
	{
		MouseLocatedZone = NotInNode;
		
		SNode::OnMouseLeave(MouseEvent);
	}
}

FCursorReply SGraphNodeCustomizableComment::OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const
{
	if (MouseLocatedZone == RightBorder || MouseLocatedZone == LeftBorder)
	{
		return FCursorReply::Cursor(EMouseCursor::ResizeLeftRight);
	}
	
	if (MouseLocatedZone == BottomRightBorder || MouseLocatedZone == TopLeftBorder)
	{
		return FCursorReply::Cursor(EMouseCursor::ResizeSouthEast);
	}
	
	if (MouseLocatedZone == BottomBorder || MouseLocatedZone == TopBorder)
	{
		return FCursorReply::Cursor(EMouseCursor::ResizeUpDown);
	}

	if (MouseLocatedZone == BottomLeftBorder || MouseLocatedZone == TopRightBorder)
	{
		return FCursorReply::Cursor(EMouseCursor::ResizeSouthWest);
	}

	if (MouseLocatedZone == TitleBar)
	{
		return FCursorReply::Cursor(GetTitleBarMouseCursor());
	}

	return FCursorReply::Unhandled();
}

FReply SGraphNodeCustomizableComment::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseIsInSelectionZone() && MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && IsEditable.Get()) 
	{
		bUserIsDragging = true;
		StoredUserSize = UserSize;
		DragSize = UserSize;

		UpdateAnchorPoint();

		return FReply::Handled().CaptureMouse(SharedThis(this));
	}
	
	return FReply::Unhandled();
}

FReply SGraphNodeCustomizableComment::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bUserIsDragging)
	{
		bUserIsDragging = false;

		UserSize.X = FMath::RoundToFloat(UserSize.X);
		UserSize.Y = FMath::RoundToFloat(UserSize.Y);

		GetNodeObj()->ResizeNode(UserSize);

		ResizeTransactionPtr.Reset();

		HandleSelection(bIsSelected);
		
		UpdateNodesUnderComment();

		return FReply::Handled().ReleaseMouseCapture();
	}
	
	return FReply::Unhandled();
}

FReply SGraphNodeCustomizableComment::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!bUserIsDragging)
	{
		MouseLocatedZone = GetMouseZone(MyGeometry, MouseEvent);
	
		return SGraphNode::OnMouseMove(MyGeometry, MouseEvent);
	}
	
	const FVector2D DeltaNodeSize = GetDeltaNodeSize(GetDeltaMouseCoordinates(MouseEvent));
		
	DragSize.X += DeltaNodeSize.X;
	DragSize.Y += DeltaNodeSize.Y;
		
	const FVector2D SnappedNodeSize = GetSnappedNodeSize();
	
	FVector2D DeltaNodePosition(0,0);

	if(UserSize != SnappedNodeSize)
	{
		if(MouseLocatedZone != BottomBorder && MouseLocatedZone != RightBorder && MouseLocatedZone != BottomRightBorder)
		{
			DeltaNodePosition = UserSize - SnappedNodeSize;

			if(MouseLocatedZone == BottomLeftBorder)
			{
				DeltaNodePosition.Y = 0.0f;
			}
			else if(MouseLocatedZone == TopRightBorder)
			{
				DeltaNodePosition.X = 0.0f;
			}
		}
			
		UserSize = SnappedNodeSize;
		
		GraphNode->ResizeNode(UserSize);
		
		DeltaNodePosition = GetCorrectedNodePositionByAnchorPoint() - GetPosition();
	}

	if (!ResizeTransactionPtr.IsValid() && UserSize != StoredUserSize)
	{
		// Start resize transaction
		ResizeTransactionPtr = MakeShareable(new FScopedTransaction(NSLOCTEXT("GraphEditor", "ResizeNodeAction", "Resize Node")));
	}

	FNodeSet NodeFilter;
	SGraphNode::MoveTo(GetPosition() + DeltaNodePosition, NodeFilter);
	
	return SGraphNode::OnMouseMove(MyGeometry, MouseEvent);
}

void SGraphNodeCustomizableComment::UpdateAnchorPoint()
{
	NodeAnchorPoint = GetPosition();
	
	if(MouseLocatedZone == LeftBorder || MouseLocatedZone == TopBorder || MouseLocatedZone == TopLeftBorder)
	{
		NodeAnchorPoint += UserSize;
	}
	else if(MouseLocatedZone == BottomLeftBorder)
	{
		NodeAnchorPoint.X += UserSize.X;
	}
	else if(MouseLocatedZone == TopRightBorder)
	{
		NodeAnchorPoint.Y += UserSize.Y;
	}
}

FVector2D SGraphNodeCustomizableComment::GetCorrectedNodePositionByAnchorPoint() const
{
	FVector2D CorrectedNodePositionByAnchorPoint = NodeAnchorPoint;
	
	if(MouseLocatedZone == LeftBorder || MouseLocatedZone == TopBorder || MouseLocatedZone == TopLeftBorder)
	{
		CorrectedNodePositionByAnchorPoint -= UserSize;
	}
	else if(MouseLocatedZone == BottomLeftBorder)
	{
		CorrectedNodePositionByAnchorPoint.X -= UserSize.X;
	}
	else if(MouseLocatedZone == TopRightBorder)
	{
		CorrectedNodePositionByAnchorPoint.Y -= UserSize.Y;
	}

	return CorrectedNodePositionByAnchorPoint;
}

SGraphNodeCustomizableComment::ECommentNodeZone SGraphNodeCustomizableComment::GetMouseZone(const FGeometry& InGeometry, const FPointerEvent& InPointerEvent) const
{
	const FVector2D LocalMouseCoordinates = InGeometry.AbsoluteToLocal(InPointerEvent.GetScreenSpacePosition());
	
	return  GetMouseZone(LocalMouseCoordinates);
}

SGraphNodeCustomizableComment::ECommentNodeZone SGraphNodeCustomizableComment::GetMouseZone(const FVector2D& LocalMouseCoordinates) const
{
	ECommentNodeZone MouseZone = NotInNode;
	
	const FSlateRect HitTestingSlateRect = GetHitTestingBorderRect();
	
	const FVector2D DesiredNodeSize = GetDesiredSize();

	if (LocalMouseCoordinates.Y > DesiredNodeSize.Y - HitTestingSlateRect.Bottom)
	{
		MouseZone = BottomBorder;
	}
	else if (LocalMouseCoordinates.Y <= HitTestingSlateRect.Top)
	{
		MouseZone = TopBorder;
	}
	else if (LocalMouseCoordinates.Y <= GetTitleBarHeight())
	{
		MouseZone = TitleBar;
	}

	if (LocalMouseCoordinates.X > DesiredNodeSize.X - HitTestingSlateRect.Right)
	{
		if (MouseZone == BottomBorder)
		{
			MouseZone = BottomRightBorder;
		}
		else if (MouseZone == TopBorder)
		{
			MouseZone = TopRightBorder;
		}
		else
		{
			MouseZone = RightBorder;
		}
	}
	else if (LocalMouseCoordinates.X <= HitTestingSlateRect.Left)
	{
		if (MouseZone == TopBorder)
		{
			MouseZone = TopLeftBorder;
		}
		else if (MouseZone == BottomBorder)
		{
			MouseZone = BottomLeftBorder;
		}
		else
		{
			MouseZone = LeftBorder;
		}
	}
	
	if (MouseZone == NotInNode)
	{
		if (LocalMouseCoordinates.Y > HitTestingSlateRect.Top) 
		{
			MouseZone = InNode;
		}
		else if (LocalMouseCoordinates.X > HitTestingSlateRect.Left) 
		{
			MouseZone = InNode;
		}
	}
	
	return MouseZone;
}

float SGraphNodeCustomizableComment::GetTitleBarHeight() const
{
	return TitleBarBorder.IsValid() ? TitleBarBorder->GetDesiredSize().Y : 0.0f;
}

FVector2D SGraphNodeCustomizableComment::GetMinNodeSize()
{
	return CustomizableCommentNodeDefinitions::MinNodeSize;
}

FVector2D SGraphNodeCustomizableComment::GetMaxNodeSize() const
{
	return FVector2D(UserSize.X + 100, UserSize.Y + 100);
}

FSlateRect SGraphNodeCustomizableComment::GetHitTestingBorderRect()
{
	return CustomizableCommentNodeDefinitions::HitTestingBorderRect;
}

void SGraphNodeCustomizableComment::Construct(const FArguments& InArgs, UEdGraphNode_Comment* InNode)
{
	GraphNode = InNode;
	bIsSelected = false;

	CachedCommentTitle = GetNodeComment();
	CachedWidth = InNode->NodeWidth;

	this->UpdateGraphNode();

	UserSize.X = InNode->NodeWidth;
	UserSize.Y = InNode->NodeHeight;

	CacheDesiredSize(1.0f);

	MouseLocatedZone = NotInNode;
	bUserIsDragging = false;
}

void SGraphNodeCustomizableComment::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SGraphNode::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	const FString CurrentCommentTitle = GetNodeComment();
	
	if (CurrentCommentTitle != CachedCommentTitle)
	{
		CachedCommentTitle = CurrentCommentTitle;
	}

	const int32 CurrentWidth = static_cast<int32>(UserSize.X);
	
	if (CurrentWidth != CachedWidth)
	{
		CachedWidth = CurrentWidth;
	}

	const UEdGraphNode_Comment* CommentNode = CastChecked<UEdGraphNode_Comment>(GraphNode);
	
	if (bCachedBubbleVisibility != CommentNode->bCommentBubbleVisible_InDetailsPanel)
	{
		CommentBubble->UpdateBubble();
		
		bCachedBubbleVisibility = CommentNode->bCommentBubbleVisible_InDetailsPanel;
	}

	if (CachedFontSize != CommentNode->GetFontSize())
	{
		UpdateGraphNode();
	}
}

FReply SGraphNodeCustomizableComment::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	return FReply::Unhandled();
}

void SGraphNodeCustomizableComment::OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{

}

float SGraphNodeCustomizableComment::GetWrappingTitleTextWidth() const
{
	return CachedWidth - 32.0f;
}

void SGraphNodeCustomizableComment::UpdateRenderTransform()
{
	const FSlateRenderTransform SlateRenderTransform = Concatenate(FScale2D(CachedCommentNodeScale), FQuat2D(FMath::DegreesToRadians(CachedCommentNodeAngle)), FVector2D::ZeroVector);

	SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
	SetRenderTransform(SlateRenderTransform);
}

bool SGraphNodeCustomizableComment::IsNameReadOnly() const
{
	return !IsEditable.Get() || SGraphNode::IsNameReadOnly();
}

FSlateColor SGraphNodeCustomizableComment::GetCommentColor() const
{
	return GetCommentNodeColor();
}

void SGraphNodeCustomizableComment::UpdateGraphNode()
{
	const UEdGraphNode_Comment* CommentNode = CastChecked<UEdGraphNode_Comment>(GraphNode);

	InputPins.Empty();
	OutputPins.Empty();

	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	bCachedBubbleVisibility = CommentNode->bCommentBubbleVisible_InDetailsPanel;
	CachedFontSize = CommentNode->GetFontSize();

	SetupErrorReporting();

	FGraphNodeMetaData GraphNodeMetaData(TEXT("Graphnode"));
	PopulateMetaTag(&GraphNodeMetaData);

	CommentStyle = FAppStyle::Get().GetWidgetStyle<FInlineEditableTextBlockStyle>("Graph.CommentBlock.TitleInlineEditableText");
	CommentStyle.EditableTextBoxStyle.TextStyle.Font.Size = CachedFontSize;
	CommentStyle.TextStyle.Font.Size = CachedFontSize;

	ContentScale.Bind(this, &SGraphNode::GetContentScale);

	CreateCommentNodeWidget(GraphNodeMetaData);
}

FVector2D SGraphNodeCustomizableComment::ComputeDesiredSize(float) const
{
	return UserSize;
}

FString SGraphNodeCustomizableComment::GetNodeComment() const
{
	return GetEditableNodeTitle();
}

FReply SGraphNodeCustomizableComment::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	if(GetMouseZone(InMyGeometry, InMouseEvent) != TitleBar || !IsEditable.Get())
	{
		return FReply::Unhandled();
	}
	
	RequestRename();

	if(!HasKeyboardFocus())
	{
		FSlateApplication::Get().SetKeyboardFocus(SharedThis(this), EFocusCause::SetDirectly);
	}

	return FReply::Handled();
}

int32 SGraphNodeCustomizableComment::GetSortDepth() const
{
	const UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(GraphNode);
	
	return IsValid(CommentNode) ? CommentNode->CommentDepth : -1;
}

void SGraphNodeCustomizableComment::HandleSelection(const bool bInIsSelected) const
{
	if (GetDesiredSize().IsZero())
	{
		return;
	}
	
	if (bIsSelected || !bInIsSelected)
	{
		bIsSelected = bInIsSelected;

		return;
	}

	UpdateNodesUnderComment();

	bIsSelected = bInIsSelected;
}

void SGraphNodeCustomizableComment::UpdateNodesUnderComment() const
{
	const SGraphNodeCustomizableComment* CommentNodeWidget = const_cast<SGraphNodeCustomizableComment*>(this);
	
	UEdGraphNode_Comment* CommentNode = CastChecked<UEdGraphNode_Comment>(GraphNode);
			
	FChildren* GraphPanelChildren = CommentNodeWidget->GetOwnerPanel()->GetAllChildren();
	
	CommentNode->ClearNodesUnderComment();

	for (int32 i = 0; i < GraphPanelChildren->Num(); ++i)
	{
		const TSharedRef<SGraphNode> ChildNodeWidget = StaticCastSharedRef<SGraphNode>(GraphPanelChildren->GetChildAt(i));
		
		UObject* ChildNodeObject = ChildNodeWidget->GetObjectBeingDisplayed();
		
		if (ChildNodeObject != CommentNode && IsNodeUnderComment(ChildNodeWidget))
		{
			CommentNode->AddNodeUnderComment(ChildNodeObject);
		}
	}
}

UObject* SGraphNodeCustomizableComment::GetLastOuter() const
{
	const UObject* Package = GraphNode->GetOutermost();
	UObject* LastOuter = GraphNode->GetOuter();
	
	while (LastOuter->GetOuter() != Package)
	{
		LastOuter = LastOuter->GetOuter();
	}

	return LastOuter;
}

FVector2D SGraphNodeCustomizableComment::GetDeltaNodeSize(const FVector2D& InDeltaMouseCoordinates) const
{
	FVector2D DeltaNodeSize = InDeltaMouseCoordinates;

	if(MouseLocatedZone == LeftBorder || MouseLocatedZone == TopBorder || MouseLocatedZone == TopLeftBorder)
	{
		DeltaNodeSize = -DeltaNodeSize;
	}
	else if(MouseLocatedZone == TopRightBorder)
	{
		DeltaNodeSize.Y = -DeltaNodeSize.Y;
	}
	else if(MouseLocatedZone == BottomLeftBorder)
	{
		DeltaNodeSize.X = -DeltaNodeSize.X;
	}

	return DeltaNodeSize;
}

FVector2D SGraphNodeCustomizableComment::GetSnappedNodeSize() const
{
	const uint32 SnapGridSize = SNodePanel::GetSnapGridSize();
	
	FVector2D SnappedNodeSize;
	
	SnappedNodeSize.X = SnapGridSize * FMath::RoundToFloat(DragSize.X / SnapGridSize);
	SnappedNodeSize.Y = SnapGridSize * FMath::RoundToFloat(DragSize.Y / SnapGridSize);

	const FVector2D NodeMinSize = GetMinNodeSize();
	const FVector2D NodeMaxSize = GetMaxNodeSize();
	
	SnappedNodeSize.X = FMath::Min(SnappedNodeSize.X, NodeMaxSize.X);
	SnappedNodeSize.X = FMath::Max(SnappedNodeSize.X, NodeMinSize.X);
	
	SnappedNodeSize.Y = FMath::Min(SnappedNodeSize.Y, NodeMaxSize.Y);
	SnappedNodeSize.Y = FMath::Max(SnappedNodeSize.Y, NodeMinSize.Y);
	
	return SnappedNodeSize;
}

FVector2D SGraphNodeCustomizableComment::GetDeltaMouseCoordinates(const FPointerEvent& InPointerEvent)
{
	const FVector2D CurrentGraphSpaceMouseCoordinates = NodeCoordToGraphCoord(InPointerEvent.GetScreenSpacePosition());
	const FVector2D LastGraphSpaceMouseCoordinates = NodeCoordToGraphCoord(InPointerEvent.GetLastScreenSpacePosition());
	
	const TSharedPtr<SWindow> OwnerWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
	
	FVector2D DeltaMouseCoordinates = (CurrentGraphSpaceMouseCoordinates - LastGraphSpaceMouseCoordinates) / (OwnerWindow.IsValid() ? OwnerWindow->GetDPIScaleFactor() : 1.0f);

	if(MouseLocatedZone == LeftBorder || MouseLocatedZone == RightBorder)
	{
		DeltaMouseCoordinates.Y = 0.0f;
	}
	else if(MouseLocatedZone == TopBorder || MouseLocatedZone == BottomBorder)
	{
		DeltaMouseCoordinates.X = 0.0f;
	}

	return DeltaMouseCoordinates;
}

FSlateRect SGraphNodeCustomizableComment::GetNodeRect(TSharedRef<const SGraphNode> InNodeWidget)
{
	const FVector2D NodePosition = InNodeWidget->GetPosition();
	const FVector2D DesiredNodeSize = InNodeWidget->GetDesiredSize();
	
	return FSlateRect(NodePosition.X, NodePosition.Y, NodePosition.X + DesiredNodeSize.X, NodePosition.Y + DesiredNodeSize.Y);
}

bool SGraphNodeCustomizableComment::IsNodeUnderComment(TSharedRef<const SGraphNode> InNodeWidget) const
{
	const FSlateRect CommentNodeRect = GetNodeRect(SharedThis(this));
	const FSlateRect TargetNodeRect = GetNodeRect(InNodeWidget);
	
	return FSlateRect::IsRectangleContained(CommentNodeRect, TargetNodeRect);
}

const FSlateBrush* SGraphNodeCustomizableComment::GetShadowBrush(bool bSelected) const
{
	HandleSelection(bSelected);
	
	return SGraphNode::GetShadowBrush(bSelected);
}

void SGraphNodeCustomizableComment::GetOverlayBrushes(bool bSelected, const FVector2D WidgetSize, TArray<FOverlayBrushInfo>& Brushes) const
{
	constexpr float Fudge = 3.0f;

	HandleSelection(bSelected);

	FOverlayBrushInfo HandleBrush = FAppStyle::GetBrush(TEXT("Graph.Node.Comment.Handle"));

	HandleBrush.OverlayOffset.X = WidgetSize.X - HandleBrush.Brush->ImageSize.X - Fudge;
	HandleBrush.OverlayOffset.Y = WidgetSize.Y - HandleBrush.Brush->ImageSize.Y - Fudge;

	Brushes.Add(HandleBrush);
	
	return SGraphNode::GetOverlayBrushes(bSelected, WidgetSize, Brushes);
}

bool SGraphNodeCustomizableComment::ShouldAllowCulling() const
{
	return true;
}

void SGraphNodeCustomizableComment::MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, const bool bMarkDirty)
{
	const FVector2D DeltaPosition = NewPosition - GetPosition();
	
	SGraphNode::MoveTo(NewPosition, NodeFilter, bMarkDirty);
	
	const FModifierKeysState ModifierKeysState = FSlateApplication::Get().GetModifierKeys();
	
	if(ModifierKeysState.IsShiftDown())
	{
		bCachedShiftKeyIsDown = true;
		
		return;
	}

	if (bCachedShiftKeyIsDown)
	{
		bCachedShiftKeyIsDown = false;
		
		UpdateNodesUnderComment();
	}
	
	const UEdGraphNode_Comment* CommentNode = CastChecked<UEdGraphNode_Comment>(GraphNode);
	
	if (CommentNode->MoveMode != ECommentBoxMode::GroupMovement)
	{
		return;
	}
	
	const TSharedPtr<SGraphPanel> OwnerGraphPanel = GetOwnerPanel();

	for (FCommentNodeSet::TConstIterator NodeIterator(CommentNode->GetNodesUnderComment()); NodeIterator; ++NodeIterator)
	{
		UEdGraphNode* NodeUnderComment = Cast<UEdGraphNode>(*NodeIterator);
		
		if (!IsValid(NodeUnderComment))
		{
			continue;
		}
		
		if (OwnerGraphPanel->SelectionManager.IsNodeSelected(NodeUnderComment))
		{
			continue;
		}

		if (NodeFilter.Find(NodeUnderComment->DEPRECATED_NodeWidget.Pin()))
		{
			continue;
		}
		
		NodeFilter.Add(NodeUnderComment->DEPRECATED_NodeWidget.Pin());
		
		NodeUnderComment->Modify(bMarkDirty);
		
		NodeUnderComment->NodePosX += DeltaPosition.X;
		NodeUnderComment->NodePosY += DeltaPosition.Y;
	}
}

void SGraphNodeCustomizableComment::EndUserInteraction() const
{
	if(GetDesiredSize().IsZero())
	{
		return;
	}
	
	const FSlateRect CommentNodeRect = GetNodeRect(SharedThis(this));
	
	FChildren* GraphPanelChildren = GetOwnerPanel()->GetAllChildren();

	for (int32 i = 0; i < GraphPanelChildren->Num(); ++i)
	{
		const TSharedPtr<SGraphNode> ChildNodeWidget = StaticCastSharedRef<SGraphNode>(GraphPanelChildren->GetChildAt(i));

		const UObject* ChildNodeObject = ChildNodeWidget->GetObjectBeingDisplayed();
			
		if (!ChildNodeObject->IsA<UEdGraphNode_Comment>())
		{
			continue;
		}

		const FSlateRect ChildNodeRect = GetNodeRect(ChildNodeWidget.ToSharedRef());
		
		if (!FSlateRect::DoRectanglesIntersect(CommentNodeRect, ChildNodeRect))
		{
			continue;
		}
		
		const TSharedPtr<SGraphNodeCustomizableComment> CommentNodeWidget = StaticCastSharedPtr<SGraphNodeCustomizableComment>(ChildNodeWidget);
		
		CommentNodeWidget->HandleSelection(CommentNodeWidget->bIsSelected);

		UpdateNodesUnderComment();
	}
}

FSlateColor SGraphNodeCustomizableComment::GetTitleBarColor() const
{
	const FLinearColor TitleBarColor = GetCommentNodeColor().GetSpecifiedColor() * CustomizableCommentNodeDefinitions::TitleBarColorMultiplier;
	
	return FLinearColor(TitleBarColor.R, TitleBarColor.G, TitleBarColor.B);
}

FSlateColor SGraphNodeCustomizableComment::GetCommentBubbleColor() const
{
	const UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(GraphNode);

	FLinearColor CommentBubbleColor;

	if (!IsValid(CommentNode))
	{
		CommentBubbleColor = FLinearColor::White * CustomizableCommentNodeDefinitions::TitleBarColorMultiplier;
	}
	else if (CommentNode->bColorCommentBubble)
	{
		CommentBubbleColor = GetCommentNodeColor().GetSpecifiedColor() * CustomizableCommentNodeDefinitions::TitleBarColorMultiplier;
	}
	else
	{
		CommentBubbleColor = GetDefault<UGraphEditorSettings>()->DefaultCommentNodeTitleColor;
	}
	
	return FLinearColor(CommentBubbleColor.R, CommentBubbleColor.G, CommentBubbleColor.B);
}

EVisibility SGraphNodeCustomizableComment::GetCommentTextVisibility() const
{
	return EVisibility::Visible;
}

EVisibility SGraphNodeCustomizableComment::GetCommentBubbleVisibility() const
{
	return EVisibility::Visible;
}

EMouseCursor::Type SGraphNodeCustomizableComment::GetTitleBarMouseCursor() const
{
	return EMouseCursor::CardinalCross;
}

void SGraphNodeCustomizableComment::CreateCommentNodeWidget(const FGraphNodeMetaData& InGraphNodeMetaData)
{
	GetOrAddSlot(ENodeZone::Center)
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("Kismet.Comment.Background"))
		.ColorAndOpacity(FLinearColor::White)
		.BorderBackgroundColor(this, &SGraphNodeCustomizableComment::GetCommentNodeColor)
		.Padding(FMargin(3.0f))
		.AddMetaData<FGraphNodeMetaData>(InGraphNodeMetaData)
		[
			SNew(SVerticalBox)
			.ToolTipText(this, &SGraphNode::GetNodeTooltip)
			+SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				SAssignNew(TitleBarBorder, SBorder)
				.BorderImage(FAppStyle::GetBrush("Graph.Node.TitleBackground"))
				.BorderBackgroundColor(this, &SGraphNodeCustomizableComment::GetTitleBarColor)
				.Padding(FMargin(10,5,5,3))
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Center)
				[
					SAssignNew(InlineEditableText, SInlineEditableTextBlock)
					.Style(&CommentStyle)
					.Text(this, &SGraphNodeCustomizableComment::GetEditableNodeTitleAsText)
					.OnVerifyTextChanged(this, &SGraphNodeCustomizableComment::OnVerifyNameTextChanged)
					.OnTextCommitted(this, &SGraphNodeCustomizableComment::OnNameTextCommited)
					.IsReadOnly(this, &SGraphNodeCustomizableComment::IsNameReadOnly)
					.IsSelected(this, &SGraphNodeCustomizableComment::IsSelectedExclusively)
					.WrapTextAt(this, &SGraphNodeCustomizableComment::GetWrappingTitleTextWidth)
					.MultiLine(true)
					.ModiferKeyForNewLine(EModifierKey::Shift)
					.Visibility(this, &SGraphNodeCustomizableComment::GetCommentTextVisibility)
				]
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			.Padding(1.0f)
			[
				ErrorReporting->AsWidget()
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("NoBorder"))
			]
		]
	];

	CommentBubble = SNew(SCommentBubble)
	.GraphNode(GraphNode)
	.Text(this, &SGraphNodeCustomizableComment::GetNodeComment)
	.OnTextCommitted(this, &SGraphNodeCustomizableComment::OnNameTextCommited)
	.ColorAndOpacity(this, &SGraphNodeCustomizableComment::GetCommentBubbleColor)
	.AllowPinning(true)
	.EnableTitleBarBubble(false)
	.EnableBubbleCtrls(false)
	.GraphLOD(this, &SGraphNode::GetCurrentLOD)
	.InvertLODCulling(true)
	.IsGraphNodeHovered(this, &SGraphNode::IsHovered)
	.Visibility(this, &SGraphNodeCustomizableComment::GetCommentBubbleVisibility);

	GetOrAddSlot(ENodeZone::TopCenter)
	.SlotOffset(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetOffset))
	.SlotSize(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetSize))
	.AllowScaling(TAttribute<bool>(CommentBubble.Get(), &SCommentBubble::IsScalingAllowed))
	.VAlign(VAlign_Top)
	[
		CommentBubble.ToSharedRef()
	];
}

void SGraphNodeCustomizableComment::SetCommentNodeAngle(const float NewAngle)
{
	CachedCommentNodeAngle = NewAngle;
	
	UpdateRenderTransform();
}

void SGraphNodeCustomizableComment::SetCommentNodeScale(const float NewScale)
{
	CachedCommentNodeScale = NewScale;
	
	UpdateRenderTransform();
}

void SGraphNodeCustomizableComment::SetCommentNodeColor(const FLinearColor& NewCommentNodeColor) const
{
	UEdGraphNode_Comment* CommentNode = CastChecked<UEdGraphNode_Comment>(GraphNode);

	CommentNode->CommentColor = NewCommentNodeColor;
}

FSlateColor SGraphNodeCustomizableComment::GetCommentNodeColor() const
{
	const UEdGraphNode_Comment* CommentNode = CastChecked<UEdGraphNode_Comment>(GraphNode);

	return CommentNode->CommentColor;
}

bool SGraphNodeCustomizableComment::CanBeSelected(const FVector2D& MousePositionInNode) const
{
	return GetMouseZone(MousePositionInNode) == TitleBar;
}

FVector2D SGraphNodeCustomizableComment::GetDesiredTitleBarSize() const
{
	const float TitleBarHeight = TitleBarBorder.IsValid() ? TitleBarBorder->GetDesiredSize().Y : 0.0f;
	
	return FVector2D(UserSize.X, TitleBarHeight);
}

FSlateRect SGraphNodeCustomizableComment::GetTitleRect() const
{
	const FVector2D CommentNodePosition = GetPosition();
	const FVector2D TitleBarSize = TitleBarBorder.IsValid() ? TitleBarBorder->GetDesiredSize() : GetDesiredSize();
	
	return FSlateRect(CommentNodePosition.X, CommentNodePosition.Y, CommentNodePosition.X + TitleBarSize.X, CommentNodePosition.Y + TitleBarSize.Y) + CustomizableCommentNodeDefinitions::TitleBarOffset;
}

void SGraphNodeCustomizableComment::PopulateMetaTag(FGraphNodeMetaData* TagMeta) const
{
	if (!IsValid(GraphNode))
	{
		return;
	}

	const UObject* LastOuter = GetLastOuter();
		
	TagMeta->Tag = FName(*FString::Printf(TEXT("GraphNode_%s_%s"), *LastOuter->GetFullName(), *GraphNode->NodeGuid.ToString()));
	TagMeta->OuterName = LastOuter->GetFullName();
	TagMeta->GUID = GraphNode->NodeGuid;
	TagMeta->FriendlyName = FString::Printf(TEXT("%s in %s"), *GraphNode->GetNodeTitle(ENodeTitleType::FullTitle).ToString(), *TagMeta->OuterName);
}
