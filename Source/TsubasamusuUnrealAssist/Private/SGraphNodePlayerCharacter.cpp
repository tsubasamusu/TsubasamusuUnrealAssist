// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "SGraphNodePlayerCharacter.h"

namespace GraphNodeResizableDefs
{
	/** ウィンドウの境界線に対するヒット結果の境界線のサイズ */
	static const FSlateRect HitResultBorderSize(10, 10, 10, 10);

	/** タイトルバーのデフォルトサイズ */
	static constexpr float DefaultTitleBarHeight = 12.f;

	/** ノードの最小サイズ */
	static const FVector2D MinNodeSize(30.0f, 30.0f);

	/** ノードの最大サイズ */
	static const FVector2D MaxNodeSize(400.0f, 400.0f);
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

		return FReply::Handled().CaptureMouse( SharedThis(this) );
	}
	
	return FReply::Unhandled();
}

FReply SGraphNodePlayerCharacter::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bUserIsDragging)
	{
		bUserIsDragging = false;

		UserSize.X = FMath::RoundToFloat(UserSize.X);
		UserSize.Y = FMath::RoundToFloat(UserSize.Y);

		GetNodeObj()->ResizeNode(UserSize);

		//サイズ変更トランザクションを終了する
		ResizeTransactionPtr.Reset();

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
	else if (LocalMouseCoordinates.Y <= (HitResultBorderSize.Top))
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
	return GraphNodeResizableDefs::DefaultTitleBarHeight;
}

FVector2D SGraphNodePlayerCharacter::GetNodeMinimumSize() const
{
	return GraphNodeResizableDefs::MinNodeSize;
}

FVector2D SGraphNodePlayerCharacter::GetNodeMaximumSize() const
{
	return GraphNodeResizableDefs::MaxNodeSize;
}

FSlateRect SGraphNodePlayerCharacter::GetHitTestingBorder() const
{
	return GraphNodeResizableDefs::HitResultBorderSize;
}
