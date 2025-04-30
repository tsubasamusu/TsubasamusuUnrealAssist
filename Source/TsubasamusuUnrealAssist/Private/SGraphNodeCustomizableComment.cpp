// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "SGraphNodeCustomizableComment.h"
#include "EdGraphNode_Comment.h"
#include "SCommentBubble.h"
#include "SGraphPanel.h"
#include "TutorialMetaData.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

namespace SGraphNodeCustomizableCommentDefs
{
	/** ウィンドウの境界線に対するヒット結果の境界線のサイズ */
	static const FSlateRect HitResultBorderSize(10, 10, 10, 10);

	/** ノードの最小サイズ */
	static const FVector2D MinNodeSize(30.0f, 30.0f);

	/** TitleBarColor = CommentColor * TitleBarColorMultiplier */
	static constexpr float TitleBarColorMultiplier = 0.6f;

	/** Titlebar Offset - taken from the widget borders in UpdateGraphNode */
	static const FSlateRect TitleBarOffset(13,8,-3,0);
}

bool SGraphNodeCustomizableComment::InSelectionArea() const
{
	return InSelectionArea(MouseZone);
}

bool SGraphNodeCustomizableComment::InSelectionArea(const EResizableWindowZone InMouseZone)
{
	return InMouseZone == RightBorder || InMouseZone == BottomBorder || InMouseZone == BottomRightBorder || InMouseZone == LeftBorder || InMouseZone == TopBorder || InMouseZone == TopLeftBorder || InMouseZone == TopRightBorder || InMouseZone == BottomLeftBorder;
}

void SGraphNodeCustomizableComment::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	//マウスがいるゾーンを決定する
	if(!bUserIsDragging)
	{
		const FVector2D LocalMouseCoordinates = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
		MouseZone = FindMouseZone(LocalMouseCoordinates);
		SNode::OnMouseEnter(MyGeometry, MouseEvent);
	}
}

void SGraphNodeCustomizableComment::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	if(!bUserIsDragging)
	{
		//マウスゾーンをリセットする
		MouseZone = NotInWindow;
		SNode::OnMouseLeave(MouseEvent);
	}
}

FCursorReply SGraphNodeCustomizableComment::OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const
{
	if (MouseZone == RightBorder || MouseZone == LeftBorder)
	{
		return FCursorReply::Cursor(EMouseCursor::ResizeLeftRight);
	}
	
	if (MouseZone == BottomRightBorder || MouseZone == TopLeftBorder)
	{
		return FCursorReply::Cursor(EMouseCursor::ResizeSouthEast);
	}
	
	if (MouseZone == BottomBorder || MouseZone == TopBorder)
	{
		return FCursorReply::Cursor(EMouseCursor::ResizeUpDown);
	}

	if (MouseZone == BottomLeftBorder || MouseZone == TopRightBorder)
	{
		return FCursorReply::Cursor(EMouseCursor::ResizeSouthWest);
	}

	if (MouseZone == TitleBar)
	{
		return FCursorReply::Cursor(EMouseCursor::CardinalCross);
	}

	return FCursorReply::Unhandled();
}

FReply SGraphNodeCustomizableComment::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (InSelectionArea() && MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && IsEditable.Get()) 
	{
		bUserIsDragging = true;
		StoredUserSize = UserSize;
		DragSize = UserSize;

		//ノードのアンカーポイントを見つける
		InitNodeAnchorPoint();

		return FReply::Handled().CaptureMouse(SharedThis(this));
	}
	
	return FReply::Unhandled();
}

FReply SGraphNodeCustomizableComment::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bUserIsDragging)
	{
		bUserIsDragging = false;

		//ノードのサイズを変更する
		UserSize.X = FMath::RoundToFloat(UserSize.X);
		UserSize.Y = FMath::RoundToFloat(UserSize.Y);

		GetNodeObj()->ResizeNode(UserSize);

		//リサイズトランザクションを終了する
		ResizeTransactionPtr.Reset();

		//含まれる子ノードを更新する
		HandleSelection(bIsSelected, true);

		return FReply::Handled().ReleaseMouseCapture();
	}
	
	return FReply::Unhandled();
}

FReply SGraphNodeCustomizableComment::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (bUserIsDragging)
	{
		const FVector2D GraphSpaceCoordinates = NodeCoordToGraphCoord(MouseEvent.GetScreenSpacePosition());
		const FVector2D OldGraphSpaceCoordinates = NodeCoordToGraphCoord(MouseEvent.GetLastScreenSpacePosition());
		const TSharedPtr<SWindow> OwnerWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
		FVector2D Delta = (GraphSpaceCoordinates - OldGraphSpaceCoordinates) / (OwnerWindow.IsValid() ? OwnerWindow->GetDPIScaleFactor() : 1.0f);
		
		//リサイズ方向に基づくクランプデルタ値
		if(MouseZone == LeftBorder || MouseZone == RightBorder)
		{
			Delta.Y = 0.0f;
		}
		else if(MouseZone == TopBorder || MouseZone == BottomBorder)
		{
			Delta.X = 0.0f;
		}

		//ノードのデルタ値をリサイズする
		FVector2D DeltaNodeSize = Delta;

		//リサイズ方向に基づいてノードサイズのデルタ値を変更する
		if(MouseZone == LeftBorder || MouseZone == TopBorder || MouseZone == TopLeftBorder)
		{
			DeltaNodeSize = -DeltaNodeSize;
		}
		else if(MouseZone == TopRightBorder)
		{
			DeltaNodeSize.Y = -DeltaNodeSize.Y;
		}
		else if(MouseZone == BottomLeftBorder)
		{
			DeltaNodeSize.X = -DeltaNodeSize.X;
		}
		
		//フィルターなしのデルタを DragSize に適用する。
		DragSize.X += DeltaNodeSize.X;
		DragSize.Y += DeltaNodeSize.Y;
		
		//スナップを適用する
		const uint32 SnapSize = SNodePanel::GetSnapGridSize();
		FVector2D SnappedSize;
		SnappedSize.X = SnapSize * FMath::RoundToFloat(DragSize.X/SnapSize);
		SnappedSize.Y = SnapSize * FMath::RoundToFloat(DragSize.Y/SnapSize);

		//最小／最大サイジングを実施する
		const FVector2D MinSize = GetNodeMinimumSize();
		SnappedSize.X = FMath::Max(SnappedSize.X, MinSize.X);
		SnappedSize.Y = FMath::Max(SnappedSize.Y, MinSize.Y);

		const FVector2D MaxSize = GetNodeMaximumSize();
		SnappedSize.X = FMath::Min(SnappedSize.X, MaxSize.X);
		SnappedSize.Y = FMath::Min(SnappedSize.Y, MaxSize.Y);

		FVector2D DeltaNodePos(0,0);

		if(UserSize != SnappedSize)
		{
			//ノードの位置を変更する（上側と左側のサイズを変更する）
			if(MouseZone != BottomBorder && MouseZone != RightBorder && MouseZone != BottomRightBorder)
			{
				//グラフノードの位置を移動するデルタ値
				DeltaNodePos = UserSize - SnappedSize;

				//リサイズ方向に基づくクランプ位置のデルタ
				if(MouseZone == BottomLeftBorder)
				{
					DeltaNodePos.Y = 0.0f;
				}
				else if(MouseZone == TopRightBorder)
				{
					DeltaNodePos.X = 0.0f;
				}
			}
			
			UserSize = SnappedSize;
			GraphNode->ResizeNode(UserSize);
			DeltaNodePos = GetCorrectedNodePosition() - GetPosition();
		}

		if (!ResizeTransactionPtr.IsValid() && UserSize != StoredUserSize)
		{
			//サイズ変更トランザクションを開始する
			//トランザクションはここで開始されるため、すべての MoveTo アクションが捕捉される
			//トランザクションは作成されない。
			ResizeTransactionPtr = MakeShareable(new FScopedTransaction(NSLOCTEXT("GraphEditor", "ResizeNodeAction", "Resize Node")));
		}

		FNodeSet NodeFilter;
		SGraphNode::MoveTo(GetPosition() + DeltaNodePos, NodeFilter);
	}
	else
	{
		const FVector2D LocalMouseCoordinates = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
		MouseZone = FindMouseZone(LocalMouseCoordinates);
	}
	
	return SGraphNode::OnMouseMove(MyGeometry, MouseEvent);
}

void SGraphNodeCustomizableComment::InitNodeAnchorPoint()
{
	NodeAnchorPoint = GetPosition();
	
	if(MouseZone == LeftBorder || MouseZone == TopBorder || MouseZone == TopLeftBorder)
	{
		NodeAnchorPoint += UserSize;
	}
	else if(MouseZone == BottomLeftBorder)
	{
		NodeAnchorPoint.X += UserSize.X;
	}
	else if(MouseZone == TopRightBorder)
	{
		NodeAnchorPoint.Y += UserSize.Y;
	}
}

FVector2D SGraphNodeCustomizableComment::GetCorrectedNodePosition() const
{
	FVector2D CorrectedPos = NodeAnchorPoint;
	
	if(MouseZone == LeftBorder || MouseZone == TopBorder || MouseZone == TopLeftBorder)
	{
		CorrectedPos -= UserSize;
	}
	else if(MouseZone == BottomLeftBorder)
	{
		CorrectedPos.X -= UserSize.X;
	}
	else if(MouseZone == TopRightBorder)
	{
		CorrectedPos.Y -= UserSize.Y;
	}

	return CorrectedPos;
}

SGraphNodeCustomizableComment::EResizableWindowZone SGraphNodeCustomizableComment::FindMouseZone(const FVector2D& LocalMouseCoordinates) const
{
	EResizableWindowZone OutMouseZone = NotInWindow;
	const FSlateRect HitResultBorderSize = GetHitTestingBorder();
	const FVector2D NodeSize = GetDesiredSize();

	//掴むゾーンの位置でのヒットテスト
	if (LocalMouseCoordinates.Y > NodeSize.Y - HitResultBorderSize.Bottom)
	{
		OutMouseZone = BottomBorder;
	}
	else if (LocalMouseCoordinates.Y <= HitResultBorderSize.Top)
	{
		OutMouseZone = TopBorder;
	}
	else if (LocalMouseCoordinates.Y <= GetTitleBarHeight())
	{
		OutMouseZone = TitleBar;
	}

	if (LocalMouseCoordinates.X > NodeSize.X - HitResultBorderSize.Right)
	{
		if (OutMouseZone == BottomBorder)
		{
			OutMouseZone = BottomRightBorder;
		}
		else if (OutMouseZone == TopBorder)
		{
			OutMouseZone = TopRightBorder;
		}
		else
		{
			OutMouseZone = RightBorder;
		}
	}
	else if (LocalMouseCoordinates.X <= HitResultBorderSize.Left)
	{
		if (OutMouseZone == TopBorder)
		{
			OutMouseZone = TopLeftBorder;
		}
		else if (OutMouseZone == BottomBorder)
		{
			OutMouseZone = BottomLeftBorder;
		}
		else
		{
			OutMouseZone = LeftBorder;
		}
	}
	
	//残りのフレームへのヒットテスト
	if (OutMouseZone == NotInWindow)
	{
		if (LocalMouseCoordinates.Y > HitResultBorderSize.Top) 
		{
			OutMouseZone = InWindow;
		}
		else if (LocalMouseCoordinates.X > HitResultBorderSize.Left) 
		{
			OutMouseZone = InWindow;
		}
	}
	
	return OutMouseZone;
}

float SGraphNodeCustomizableComment::GetTitleBarHeight() const
{
	return TitleBarBorder.IsValid() ? TitleBarBorder->GetDesiredSize().Y : 0.0f;
}

FVector2D SGraphNodeCustomizableComment::GetNodeMinimumSize() const
{
	return SGraphNodeCustomizableCommentDefs::MinNodeSize;
}

FVector2D SGraphNodeCustomizableComment::GetNodeMaximumSize() const
{
	return FVector2D(UserSize.X + 100, UserSize.Y + 100);
}

FSlateRect SGraphNodeCustomizableComment::GetHitTestingBorder() const
{
	return SGraphNodeCustomizableCommentDefs::HitResultBorderSize;
}

void SGraphNodeCustomizableComment::Construct(const FArguments& InArgs, UEdGraphNode_Comment* InNode)
{
	this->GraphNode = InNode;
	this->bIsSelected = false;

	//アニメーションをセットアップする
	ZoomCurve = SpawnAnim.AddCurve(0, 0.1f);
	FadeCurve = SpawnAnim.AddCurve(0.15f, 0.15f);

	//これらの値をキャッシュしておけば、次のティックにノードの再ビルドを強いられることはない
	CachedCommentTitle = GetNodeComment();
	CachedWidth = InNode->NodeWidth;

	this->UpdateGraphNode();

	//サイズを引き出す
	UserSize.X = InNode->NodeWidth;
	UserSize.Y = InNode->NodeHeight;

	//希望するサイズをキャッシュし、正しくカリングできるようにする
	//ComputeDesiredSize はレイアウトのスケールを無視するので、これを行うことができます
	CacheDesiredSize(1.0f);

	MouseZone = NotInWindow;
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

	UEdGraphNode_Comment* CommentNode = CastChecked<UEdGraphNode_Comment>(GraphNode);
	
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

float SGraphNodeCustomizableComment::GetWrapAt() const
{
	return CachedWidth - 32.0f;
}

bool SGraphNodeCustomizableComment::IsNameReadOnly() const
{
	return !IsEditable.Get() || SGraphNode::IsNameReadOnly();
}

FSlateColor SGraphNodeCustomizableComment::GetCommentColor() const
{
	return GetCommentBodyColor();
}

void SGraphNodeCustomizableComment::UpdateGraphNode()
{
	//コメントボックスにピンは無い
	InputPins.Empty();
	OutputPins.Empty();

	//標準的なボックスモデルも避ける
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	//コメントバブルを見せるべきかどうか
	const UEdGraphNode_Comment* CommentNode = CastChecked<UEdGraphNode_Comment>(GraphNode);
	bCachedBubbleVisibility = CommentNode->bCommentBubbleVisible_InDetailsPanel;

	//このノードのタグを設定する
	FString TagName;

	//ブループリントの名前を名前にしたいなら、GUID からノードを見つける事が出来る
	UObject* Package = GraphNode->GetOutermost();
	UObject* LastOuter = GraphNode->GetOuter();
	
	while (LastOuter->GetOuter() != Package)
	{
		LastOuter = LastOuter->GetOuter();
	}
	
	TagName = FString::Printf(TEXT("GraphNode,%s,%s"), *LastOuter->GetFullName(), *GraphNode->NodeGuid.ToString());

	SetupErrorReporting();

	//このノードにメタタグを設定する
	FGraphNodeMetaData TagMeta(TEXT("Graphnode"));
	PopulateMetaTag(&TagMeta);

	CachedFontSize = CommentNode->GetFontSize();

	CommentStyle = FAppStyle::Get().GetWidgetStyle<FInlineEditableTextBlockStyle>("Graph.CommentBlock.TitleInlineEditableText");
	CommentStyle.EditableTextBoxStyle.TextStyle.Font.Size = CachedFontSize;
	CommentStyle.TextStyle.Font.Size = CachedFontSize;

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Kismet.Comment.Background"))
			.ColorAndOpacity(FLinearColor::White)
			.BorderBackgroundColor(this, &SGraphNodeCustomizableComment::GetCommentBodyColor)
			.Padding(FMargin(3.0f))
			.AddMetaData<FGraphNodeMetaData>(TagMeta)
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
					.BorderBackgroundColor(this, &SGraphNodeCustomizableComment::GetCommentTitleBarColor)
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
						.WrapTextAt(this, &SGraphNodeCustomizableComment::GetWrapAt)
						.MultiLine(true)
						.ModiferKeyForNewLine(EModifierKey::Shift)
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
					//ノードのコンテンツの領域
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("NoBorder"))
				]
			]
		];

	//コメントバブルを作成する
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
	.IsGraphNodeHovered(this, &SGraphNode::IsHovered);

	GetOrAddSlot(ENodeZone::TopCenter)
	.SlotOffset(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetOffset))
	.SlotSize(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetSize))
	.AllowScaling(TAttribute<bool>(CommentBubble.Get(), &SCommentBubble::IsScalingAllowed))
	.VAlign(VAlign_Top)
	[
		CommentBubble.ToSharedRef()
	];
}

FVector2D SGraphNodeCustomizableComment::ComputeDesiredSize(float) const
{
	return UserSize;
}

FString SGraphNodeCustomizableComment::GetNodeComment() const
{
	const FString Title = GetEditableNodeTitle();
	return Title;
}

FReply SGraphNodeCustomizableComment::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	//タイトルバー領域をダブルクリックしたら
	if(FindMouseZone(InMyGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition())) == TitleBar && IsEditable.Get())
	{
		//リネームを要求する
		RequestRename();

		//キーボードフォーカスを設定する
		if(!HasKeyboardFocus())
		{
			FSlateApplication::Get().SetKeyboardFocus(SharedThis(this), EFocusCause::SetDirectly);
		}

		return FReply::Handled();
	}
	
	//そうでなければ、コメントノードと重なったときにスプラインのインタラクションが機能するようにグラフに処理させる
	return FReply::Unhandled();
}

int32 SGraphNodeCustomizableComment::GetSortDepth() const
{
	const UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(GraphNode);
	return CommentNode ? CommentNode->CommentDepth : -1;
}

void SGraphNodeCustomizableComment::HandleSelection(const bool bSelected, const bool bUpdateNodesUnderComment) const
{
	const FVector2D NodeSize = GetDesiredSize();
	
	//コメントを有効な希望サイズにした後にのみ、これを実行したい
	if(!NodeSize.IsZero())
	{
		if ((!this->bIsSelected && bSelected) || bUpdateNodesUnderComment)
		{
			const SGraphNodeCustomizableComment* Comment = const_cast<SGraphNodeCustomizableComment*> (this);
			UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(GraphNode);
			
			if (CommentNode)
			{
				const TSharedPtr<SGraphPanel> Panel = Comment->GetOwnerPanel();
				FChildren* PanelChildren = Panel->GetAllChildren();
				const int32 NumChildren = PanelChildren->Num();
				CommentNode->ClearNodesUnderComment();

				for (int32 NodeIndex = 0; NodeIndex < NumChildren; ++NodeIndex)
				{
					const TSharedRef<SGraphNode> SomeNodeWidget = StaticCastSharedRef<SGraphNode>(PanelChildren->GetChildAt(NodeIndex));
					UObject* GraphObject = SomeNodeWidget->GetObjectBeingDisplayed();
					if (GraphObject != CommentNode)
					{
						if (IsNodeUnderComment(CommentNode, SomeNodeWidget))
						{
							CommentNode->AddNodeUnderComment(GraphObject);
						}
					}
				}
			}
		}
		bIsSelected = bSelected;
	}
}

bool SGraphNodeCustomizableComment::IsNodeUnderComment(UEdGraphNode_Comment* InCommentNode, const TSharedRef<SGraphNode> InNodeWidget) const
{
	const FVector2D NodePosition = GetPosition();
	const FVector2D NodeSize = GetDesiredSize();
	const FSlateRect CommentRect(NodePosition.X, NodePosition.Y, NodePosition.X + NodeSize.X, NodePosition.Y + NodeSize.Y);

	const FVector2D InNodePosition = InNodeWidget->GetPosition();
	const FVector2D InNodeSize = InNodeWidget->GetDesiredSize();

	const FSlateRect NodeGeometryGraphSpace(InNodePosition.X, InNodePosition.Y, InNodePosition.X + InNodeSize.X, InNodePosition.Y + InNodeSize.Y);
	return FSlateRect::IsRectangleContained(CommentRect, NodeGeometryGraphSpace);
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
	const FVector2D PositionDelta = NewPosition - GetPosition();
	SGraphNode::MoveTo(NewPosition, NodeFilter, bMarkDirty);
	
	//シフトキーのどちらかが押されているときは、ノートの内容をドラッグしないでください。
	const FModifierKeysState KeysState = FSlateApplication::Get().GetModifierKeys();
	
	if(!KeysState.IsShiftDown())
	{
		const UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(GraphNode);
		if (CommentNode && CommentNode->MoveMode == ECommentBoxMode::GroupMovement)
		{
			//コメントに触れているが選択はされていないノードを更新する
			//選択されたノードは、通常の選択コードの一部として移動される
			const TSharedPtr<SGraphPanel> Panel = GetOwnerPanel();

			for (FCommentNodeSet::TConstIterator NodeIt(CommentNode->GetNodesUnderComment()); NodeIt; ++NodeIt)
			{
				if (UEdGraphNode* Node = Cast<UEdGraphNode>(*NodeIt))
				{
					if (!Panel->SelectionManager.IsNodeSelected(Node) && !NodeFilter.Find(Node->DEPRECATED_NodeWidget.Pin()))
					{
						NodeFilter.Add(Node->DEPRECATED_NodeWidget.Pin());
						Node->Modify(bMarkDirty);
						Node->NodePosX += PositionDelta.X;
						Node->NodePosY += PositionDelta.Y;
					}
				}
			}
		}
	}
}

void SGraphNodeCustomizableComment::EndUserInteraction() const
{
	//親ノードのコメントと子ノードのリストを見つける
	const FVector2D NodeSize = GetDesiredSize();
	
	if(!NodeSize.IsZero())
	{
		const FVector2D NodePosition = GetPosition();
		const FSlateRect CommentRect(NodePosition.X, NodePosition.Y, NodePosition.X + NodeSize.X, NodePosition.Y + NodeSize.Y);

		const TSharedPtr<SGraphPanel> Panel = GetOwnerPanel();
		FChildren* PanelChildren = Panel->GetAllChildren();
		const int32 NumChildren = PanelChildren->Num();
		static FString SGraphNodeCustomizableCommentType = "SGraphNodeCustomizableComment";

		for (int32 NodeIndex = 0; NodeIndex < NumChildren; ++NodeIndex)
		{
			const TSharedPtr<SGraphNode> SomeNodeWidget = StaticCastSharedRef<SGraphNode>(PanelChildren->GetChildAt(NodeIndex));

			const UObject* GraphObject = SomeNodeWidget->GetObjectBeingDisplayed();
			
			if (!GraphObject->IsA<UEdGraphNode_Comment>())
			{
				continue;
			}

			const FVector2D SomeNodePosition = SomeNodeWidget->GetPosition();
			const FVector2D SomeNodeSize = SomeNodeWidget->GetDesiredSize();

			const FSlateRect NodeGeometryGraphSpace(SomeNodePosition.X, SomeNodePosition.Y, SomeNodePosition.X + SomeNodeSize.X, SomeNodePosition.Y + SomeNodeSize.Y);

			if (FSlateRect::DoRectanglesIntersect(CommentRect, NodeGeometryGraphSpace))
			{
				// GraphObject がコメントノードであることを確認したので、このダウンキャストはこの時点で有効であるはずである
				const TSharedPtr<SGraphNodeCustomizableComment> CommentWidget = StaticCastSharedPtr<SGraphNodeCustomizableComment>(SomeNodeWidget);
				CommentWidget->HandleSelection(CommentWidget->bIsSelected, true);
			}
		}
	}
}

FSlateColor SGraphNodeCustomizableComment::GetCommentBodyColor() const
{
	const UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(GraphNode);

	if (CommentNode)
	{
		return CommentNode->CommentColor;
	}
	
	return FLinearColor::White;
}

FSlateColor SGraphNodeCustomizableComment::GetCommentTitleBarColor() const
{
	const UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(GraphNode);
	
	if (CommentNode)
	{
		const FLinearColor Color = CommentNode->CommentColor * SGraphNodeCustomizableCommentDefs::TitleBarColorMultiplier;
		return FLinearColor(Color.R, Color.G, Color.B);
	}
	
	const FLinearColor Color = FLinearColor::White * SGraphNodeCustomizableCommentDefs::TitleBarColorMultiplier;
	return FLinearColor(Color.R, Color.G, Color.B);
}

FSlateColor SGraphNodeCustomizableComment::GetCommentBubbleColor() const
{
	const UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(GraphNode);
	
	if (CommentNode)
	{
		const FLinearColor Color = CommentNode->bColorCommentBubble ? CommentNode->CommentColor * SGraphNodeCustomizableCommentDefs::TitleBarColorMultiplier : GetDefault<UGraphEditorSettings>()->DefaultCommentNodeTitleColor;
		return FLinearColor(Color.R, Color.G, Color.B);
	}
	
	const FLinearColor Color = FLinearColor::White * SGraphNodeCustomizableCommentDefs::TitleBarColorMultiplier;
	return FLinearColor(Color.R, Color.G, Color.B);
}

bool SGraphNodeCustomizableComment::CanBeSelected(const FVector2D& MousePositionInNode) const
{
	const EResizableWindowZone InMouseZone = FindMouseZone(MousePositionInNode);
	return TitleBar == InMouseZone;
}

FVector2D SGraphNodeCustomizableComment::GetDesiredSizeForMarquee() const
{
	const float TitleBarHeight = TitleBarBorder.IsValid() ? TitleBarBorder->GetDesiredSize().Y : 0.0f;
	return FVector2D(UserSize.X, TitleBarHeight);
}

FSlateRect SGraphNodeCustomizableComment::GetTitleRect() const
{
	const FVector2D NodePosition = GetPosition();
	const FVector2D NodeSize  = TitleBarBorder.IsValid() ? TitleBarBorder->GetDesiredSize() : GetDesiredSize();
	return FSlateRect(NodePosition.X, NodePosition.Y, NodePosition.X + NodeSize.X, NodePosition.Y + NodeSize.Y) + SGraphNodeCustomizableCommentDefs::TitleBarOffset;
}

void SGraphNodeCustomizableComment::PopulateMetaTag(FGraphNodeMetaData* TagMeta) const
{
	if (GraphNode != nullptr)
	{
		//ブループリントの名前を名前にしたいなら、GUID からノードを見つける事が出来る
		const UObject* Package = GraphNode->GetOutermost();
		const UObject* LastOuter = GraphNode->GetOuter();
		
		while (LastOuter->GetOuter() != Package)
		{
			LastOuter = LastOuter->GetOuter();
		}
		
		TagMeta->Tag = FName(*FString::Printf(TEXT("GraphNode_%s_%s"), *LastOuter->GetFullName(), *GraphNode->NodeGuid.ToString()));
		TagMeta->OuterName = LastOuter->GetFullName();
		TagMeta->GUID = GraphNode->NodeGuid;
		TagMeta->FriendlyName = FString::Printf(TEXT("%s in %s"), *GraphNode->GetNodeTitle(ENodeTitleType::FullTitle).ToString(), *TagMeta->OuterName);
	}
}
