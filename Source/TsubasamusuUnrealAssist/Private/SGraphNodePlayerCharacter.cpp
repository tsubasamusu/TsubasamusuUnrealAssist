// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "SGraphNodePlayerCharacter.h"
#include "EdGraphNode_Comment.h"
#include "SCommentBubble.h"
#include "SGraphPanel.h"
#include "TutorialMetaData.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

namespace SGraphNodePlayerCharacterDefs
{
	/** ウィンドウの境界線に対するヒット結果の境界線のサイズ */
	static const FSlateRect HitResultBorderSize(10, 10, 10, 10);

	/** タイトルバーのデフォルトサイズ */
	static constexpr float DefaultTitleBarHeight = 12.f;

	/** ノードの最小サイズ */
	static const FVector2D MinNodeSize(30.0f, 30.0f);

	/** ノードの最大サイズ */
	static const FVector2D MaxNodeSize(400.0f, 400.0f);

	/** コメントの最小リサイズ幅 */
	static constexpr float MinWidth = 30.0;

	/** コメントの最小リサイズ高さ */
	static constexpr float MinHeight = 30.0;

	/** TitleBarColor = CommentColor * TitleBarColorMultiplier */
	static constexpr float TitleBarColorMultiplier = 0.6f;

	/** Titlebar Offset - taken from the widget borders in UpdateGraphNode */
	static const FSlateRect TitleBarOffset(13,8,-3,0);
}

bool SGraphNodePlayerCharacter::InSelectionArea() const
{
	return InSelectionArea(MouseZone);
}

bool SGraphNodePlayerCharacter::InSelectionArea(const EResizableWindowZone InMouseZone)
{
	return InMouseZone == RightBorder || InMouseZone == BottomBorder || InMouseZone == BottomRightBorder || InMouseZone == LeftBorder || InMouseZone == TopBorder || InMouseZone == TopLeftBorder || InMouseZone == TopRightBorder || InMouseZone == BottomLeftBorder;
}

void SGraphNodePlayerCharacter::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	//マウスがいるゾーンを決定する
	if(!bUserIsDragging)
	{
		const FVector2D LocalMouseCoordinates = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
		MouseZone = FindMouseZone(LocalMouseCoordinates);
		SNode::OnMouseEnter(MyGeometry, MouseEvent);
	}
}

void SGraphNodePlayerCharacter::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	if(!bUserIsDragging)
	{
		//マウスゾーンをリセットする
		MouseZone = NotInWindow;
		SNode::OnMouseLeave(MouseEvent);
	}
}

FCursorReply SGraphNodePlayerCharacter::OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const
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

FReply SGraphNodePlayerCharacter::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
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

FReply SGraphNodePlayerCharacter::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
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

FReply SGraphNodePlayerCharacter::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
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

void SGraphNodePlayerCharacter::InitNodeAnchorPoint()
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

FVector2D SGraphNodePlayerCharacter::GetCorrectedNodePosition() const
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

SGraphNodePlayerCharacter::EResizableWindowZone SGraphNodePlayerCharacter::FindMouseZone(const FVector2D& LocalMouseCoordinates) const
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

float SGraphNodePlayerCharacter::GetTitleBarHeight() const
{
	return TitleBarBorder.IsValid() ? TitleBarBorder->GetDesiredSize().Y : 0.0f;
}

FVector2D SGraphNodePlayerCharacter::GetNodeMinimumSize() const
{
	return SGraphNodePlayerCharacterDefs::MinNodeSize;
}

FVector2D SGraphNodePlayerCharacter::GetNodeMaximumSize() const
{
	return FVector2D(UserSize.X + 100, UserSize.Y + 100);
}

FSlateRect SGraphNodePlayerCharacter::GetHitTestingBorder() const
{
	return SGraphNodePlayerCharacterDefs::HitResultBorderSize;
}

void SGraphNodePlayerCharacter::Construct(const FArguments& InArgs, UEdGraphNode_Comment* InNode)
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

void SGraphNodePlayerCharacter::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
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

FReply SGraphNodePlayerCharacter::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	return FReply::Unhandled();
}

void SGraphNodePlayerCharacter::OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{

}

float SGraphNodePlayerCharacter::GetWrapAt() const
{
	return CachedWidth - 32.0f;
}

bool SGraphNodePlayerCharacter::IsNameReadOnly() const
{
	return !IsEditable.Get() || SGraphNode::IsNameReadOnly();
}

FSlateColor SGraphNodePlayerCharacter::GetCommentColor() const
{
	return GetCommentBodyColor();
}

void SGraphNodePlayerCharacter::UpdateGraphNode()
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
			.BorderBackgroundColor(this, &SGraphNodePlayerCharacter::GetCommentBodyColor)
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
					.BorderBackgroundColor(this, &SGraphNodePlayerCharacter::GetCommentTitleBarColor)
					.Padding(FMargin(10,5,5,3))
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Center)
					[
						SAssignNew(InlineEditableText, SInlineEditableTextBlock)
						.Style(&CommentStyle)
						.Text(this, &SGraphNodePlayerCharacter::GetEditableNodeTitleAsText)
						.OnVerifyTextChanged(this, &SGraphNodePlayerCharacter::OnVerifyNameTextChanged)
						.OnTextCommitted(this, &SGraphNodePlayerCharacter::OnNameTextCommited)
						.IsReadOnly(this, &SGraphNodePlayerCharacter::IsNameReadOnly)
						.IsSelected(this, &SGraphNodePlayerCharacter::IsSelectedExclusively)
						.WrapTextAt(this, &SGraphNodePlayerCharacter::GetWrapAt)
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
	.Text(this, &SGraphNodePlayerCharacter::GetNodeComment)
	.OnTextCommitted(this, &SGraphNodePlayerCharacter::OnNameTextCommited)
	.ColorAndOpacity(this, &SGraphNodePlayerCharacter::GetCommentBubbleColor)
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

FVector2D SGraphNodePlayerCharacter::ComputeDesiredSize(float) const
{
	return UserSize;
}

FString SGraphNodePlayerCharacter::GetNodeComment() const
{
	const FString Title = GetEditableNodeTitle();
	return Title;
}

FReply SGraphNodePlayerCharacter::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
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

int32 SGraphNodePlayerCharacter::GetSortDepth() const
{
	const UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(GraphNode);
	return CommentNode ? CommentNode->CommentDepth : -1;
}

void SGraphNodePlayerCharacter::HandleSelection(const bool bSelected, const bool bUpdateNodesUnderComment) const
{
	const FVector2D NodeSize = GetDesiredSize();
	
	//コメントを有効な希望サイズにした後にのみ、これを実行したい
	if(!NodeSize.IsZero())
	{
		if ((!this->bIsSelected && bSelected) || bUpdateNodesUnderComment)
		{
			const SGraphNodePlayerCharacter* Comment = const_cast<SGraphNodePlayerCharacter*> (this);
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

bool SGraphNodePlayerCharacter::IsNodeUnderComment(UEdGraphNode_Comment* InCommentNode, const TSharedRef<SGraphNode> InNodeWidget) const
{
	const FVector2D NodePosition = GetPosition();
	const FVector2D NodeSize = GetDesiredSize();
	const FSlateRect CommentRect(NodePosition.X, NodePosition.Y, NodePosition.X + NodeSize.X, NodePosition.Y + NodeSize.Y);

	const FVector2D InNodePosition = InNodeWidget->GetPosition();
	const FVector2D InNodeSize = InNodeWidget->GetDesiredSize();

	const FSlateRect NodeGeometryGraphSpace(InNodePosition.X, InNodePosition.Y, InNodePosition.X + InNodeSize.X, InNodePosition.Y + InNodeSize.Y);
	return FSlateRect::IsRectangleContained(CommentRect, NodeGeometryGraphSpace);
}

const FSlateBrush* SGraphNodePlayerCharacter::GetShadowBrush(bool bSelected) const
{
	HandleSelection(bSelected);
	return SGraphNode::GetShadowBrush(bSelected);
}

void SGraphNodePlayerCharacter::GetOverlayBrushes(bool bSelected, const FVector2D WidgetSize, TArray<FOverlayBrushInfo>& Brushes) const
{
	constexpr float Fudge = 3.0f;

	HandleSelection(bSelected);

	FOverlayBrushInfo HandleBrush = FAppStyle::GetBrush(TEXT("Graph.Node.Comment.Handle"));

	HandleBrush.OverlayOffset.X = WidgetSize.X - HandleBrush.Brush->ImageSize.X - Fudge;
	HandleBrush.OverlayOffset.Y = WidgetSize.Y - HandleBrush.Brush->ImageSize.Y - Fudge;

	Brushes.Add(HandleBrush);
	return SGraphNode::GetOverlayBrushes(bSelected, WidgetSize, Brushes);
}

bool SGraphNodePlayerCharacter::ShouldAllowCulling() const
{
	return true;
}

void SGraphNodePlayerCharacter::MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, const bool bMarkDirty)
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

void SGraphNodePlayerCharacter::EndUserInteraction() const
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
		static FString SGraphNodePlayerCharacterType = "SGraphNodePlayerCharacter";

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
				const TSharedPtr<SGraphNodePlayerCharacter> CommentWidget = StaticCastSharedPtr<SGraphNodePlayerCharacter>(SomeNodeWidget);
				CommentWidget->HandleSelection(CommentWidget->bIsSelected, true);
			}
		}
	}
}

FSlateColor SGraphNodePlayerCharacter::GetCommentBodyColor() const
{
	const UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(GraphNode);

	if (CommentNode)
	{
		return CommentNode->CommentColor;
	}
	
	return FLinearColor::White;
}

FSlateColor SGraphNodePlayerCharacter::GetCommentTitleBarColor() const
{
	const UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(GraphNode);
	
	if (CommentNode)
	{
		const FLinearColor Color = CommentNode->CommentColor * SGraphNodePlayerCharacterDefs::TitleBarColorMultiplier;
		return FLinearColor(Color.R, Color.G, Color.B);
	}
	
	const FLinearColor Color = FLinearColor::White * SGraphNodePlayerCharacterDefs::TitleBarColorMultiplier;
	return FLinearColor(Color.R, Color.G, Color.B);
}

FSlateColor SGraphNodePlayerCharacter::GetCommentBubbleColor() const
{
	const UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(GraphNode);
	
	if (CommentNode)
	{
		const FLinearColor Color = CommentNode->bColorCommentBubble ? CommentNode->CommentColor * SGraphNodePlayerCharacterDefs::TitleBarColorMultiplier : GetDefault<UGraphEditorSettings>()->DefaultCommentNodeTitleColor;
		return FLinearColor(Color.R, Color.G, Color.B);
	}
	
	const FLinearColor Color = FLinearColor::White * SGraphNodePlayerCharacterDefs::TitleBarColorMultiplier;
	return FLinearColor(Color.R, Color.G, Color.B);
}

bool SGraphNodePlayerCharacter::CanBeSelected(const FVector2D& MousePositionInNode) const
{
	const EResizableWindowZone InMouseZone = FindMouseZone(MousePositionInNode);
	return TitleBar == InMouseZone;
}

FVector2D SGraphNodePlayerCharacter::GetDesiredSizeForMarquee() const
{
	const float TitleBarHeight = TitleBarBorder.IsValid() ? TitleBarBorder->GetDesiredSize().Y : 0.0f;
	return FVector2D(UserSize.X, TitleBarHeight);
}

FSlateRect SGraphNodePlayerCharacter::GetTitleRect() const
{
	const FVector2D NodePosition = GetPosition();
	const FVector2D NodeSize  = TitleBarBorder.IsValid() ? TitleBarBorder->GetDesiredSize() : GetDesiredSize();
	return FSlateRect(NodePosition.X, NodePosition.Y, NodePosition.X + NodeSize.X, NodePosition.Y + NodeSize.Y) + SGraphNodePlayerCharacterDefs::TitleBarOffset;
}

void SGraphNodePlayerCharacter::PopulateMetaTag(FGraphNodeMetaData* TagMeta) const
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
