// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

class SCommentBubble;
class UEdGraphNode_Comment;

class TSUBASAMUSUUNREALASSIST_API SGraphNodePlayerCharacter : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SGraphNodePlayerCharacter){}
	SLATE_END_ARGS()

	/** ユーザーが操作しているリサイズ可能なウィンドウゾーン */
	enum EResizableWindowZone
	{
		NotInWindow,
		InWindow,
		RightBorder,
		BottomBorder,
		BottomRightBorder,
		LeftBorder,
		TopBorder,
		TopLeftBorder,
		TopRightBorder,
		BottomLeftBorder,
		TitleBar
	};

	//~ Begin SWidget Interface
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FCursorReply OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const override;
	virtual FReply OnMouseButtonDoubleClick( const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent ) override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual FReply OnDrop( const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent ) override;
	virtual void OnDragEnter( const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent ) override;
	//~ End SWidget Interface
	
	//~ Begin SNodePanel::SNode Interface
	virtual const FSlateBrush* GetShadowBrush(bool bSelected) const override;
	virtual void GetOverlayBrushes(bool bSelected, const FVector2D WidgetSize, TArray<FOverlayBrushInfo>& Brushes) const override;
	virtual bool ShouldAllowCulling() const override { return true; }
	virtual int32 GetSortDepth() const override;
	virtual void EndUserInteraction() const override;
	virtual FString GetNodeComment() const override;
	//~ End SNodePanel::SNode Interface

	//~ Begin SPanel Interface
	virtual FVector2D ComputeDesiredSize(float) const override;
	//~ End SPanel Interface

	//~ Begin SGraphNode Interface
	virtual bool IsNameReadOnly() const override;
	virtual FSlateColor GetCommentColor() const override { return GetCommentBodyColor(); }
	//~ End SGraphNode Interface

	void Construct( const FArguments& InArgs, UEdGraphNode_Comment* InNode );

	/** return if the node can be selected, by pointing given location */
	virtual bool CanBeSelected( const FVector2D& MousePositionInNode ) const override;

	/** return size of the title bar */
	virtual FVector2D GetDesiredSizeForMarquee() const override;

	/** return rect of the title bar */
	virtual FSlateRect GetTitleRect() const override;

protected:

	/** マウスが現在いるウィンドウゾーンを探す */
	virtual EResizableWindowZone FindMouseZone(const FVector2D& LocalMouseCoordinates) const;

	/** @return 現在のウィンドウゾーンが選択領域とみなされる場合 true */
	bool InSelectionArea() const;

	/** @return 通過したゾーンが選択領域である場合 true */
	static bool InSelectionArea(const EResizableWindowZone InMouseZone);

	/** ノードのサイズを変更する前にアンカーポイントを保存する関数。ノードのリサイズ時にこの点にアンカーされる。*/
	void InitNodeAnchorPoint();

	/** アンカーポイントに基づいて補正されたノードの位置を取得する関数 */
	FVector2D GetCorrectedNodePosition() const;

	/** 現在のタイトルバーのサイズを取得する */
	virtual float GetTitleBarHeight() const;

	/** 希望するノードの最小サイズを返す */
	virtual FVector2D GetNodeMinimumSize() const;

	/** 希望するノードの最大サイズを返す */
	virtual FVector2D GetNodeMaximumSize() const;

	//** ヒットテスト用に FSlateRect を返す */
	virtual FSlateRect GetHitTestingBorder() const;

	//~ Begin SGraphNode Interface
	virtual void UpdateGraphNode() override;
	virtual void PopulateMetaTag(class FGraphNodeMetaData* TagMeta) const override;

	/**
	 * Helper method to update selection state of comment and any nodes 'contained' within it
	 * @param bSelected	If true comment is being selected, false otherwise
	 * @param bUpdateNodesUnderComment If true then force the rebuild of the list of nodes under the comment
	 */
	void HandleSelection(bool bIsSelected, bool bUpdateNodesUnderComment = false) const;

	/** Helper function to determine if a node is under this comment widget or not */
	virtual bool IsNodeUnderComment(UEdGraphNode_Comment* InCommentNode, const TSharedRef<SGraphNode> InNodeWidget) const;

	/** called when user is moving the comment node */
	virtual void MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty = true) override;

	/** @return the color to tint the comment body */
	FSlateColor GetCommentBodyColor() const;

	/** @return the color to tint the title bar */
	FSlateColor GetCommentTitleBarColor() const;

	/** @return the color to tint the comment bubble */
	FSlateColor GetCommentBubbleColor() const;

	/** 流体リサイズ用のノードのスナップしないサイズ */
	FVector2D DragSize;

	/** ドラッグ中に設定されるノードの希望サイズ */
	FVector2D UserSize;

	/** サイズ変更時のノードの元のサイズ */
	FVector2D StoredUserSize;

	/** サイズ変更トランザクション */
	TSharedPtr<FScopedTransaction> ResizeTransactionPtr;

	/** ノードのサイズ変更時にノードの位置を修正するためのアンカーポイント */
	FVector2D NodeAnchorPoint;

	/** マウスが現在いるウィンドウゾーン */
	EResizableWindowZone MouseZone;

	/** trueの場合、ユーザーはノードをアクティブにドラッグしている */
	bool bUserIsDragging;
	
	/** cached comment title */
	FString CachedCommentTitle;

	/** cached font size */
	int32 CachedFontSize;

	/** Was the bubble desired to be visible last frame? */
	mutable bool bCachedBubbleVisibility;

private:
	
	/** Returns the width to wrap the text of the comment at */
	float GetWrapAt() const;

	/** The comment bubble widget (used when zoomed out) */
	TSharedPtr<SCommentBubble> CommentBubble;

	/** The current selection state of the comment */
	mutable bool bIsSelected;

	/** the title bar, needed to obtain it's height */
	TSharedPtr<SBorder> TitleBarBorder;

	/** cached comment title */
	int32 CachedWidth;

	/** Local copy of the comment style */
	FInlineEditableTextBlockStyle CommentStyle;
};
