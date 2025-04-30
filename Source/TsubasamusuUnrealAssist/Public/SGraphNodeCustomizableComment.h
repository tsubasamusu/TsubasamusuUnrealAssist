// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

class SCommentBubble;
class UEdGraphNode_Comment;

class TSUBASAMUSUUNREALASSIST_API SGraphNodeCustomizableComment : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SGraphNodeCustomizableComment){}
	SLATE_END_ARGS()

	enum ECustomizableCommentNodeZone
	{
		NotInNode,
		InNode,
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
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	//~ End SWidget Interface
	
	//~ Begin SNodePanel::SNode Interface
	virtual const FSlateBrush* GetShadowBrush(bool bSelected) const override;
	virtual void GetOverlayBrushes(bool bSelected, const FVector2D WidgetSize, TArray<FOverlayBrushInfo>& Brushes) const override;
	virtual bool ShouldAllowCulling() const override;
	virtual int32 GetSortDepth() const override;
	virtual void EndUserInteraction() const override;
	virtual FString GetNodeComment() const override;
	//~ End SNodePanel::SNode Interface

	//~ Begin SPanel Interface
	virtual FVector2D ComputeDesiredSize(float) const override;
	//~ End SPanel Interface

	//~ Begin SGraphNode Interface
	virtual bool IsNameReadOnly() const override;
	virtual FSlateColor GetCommentColor() const override;
	//~ End SGraphNode Interface

	void Construct(const FArguments& InArgs, UEdGraphNode_Comment* InNode);

	/** 指定された場所を指してノードが選択可能かどうかを返す */
	virtual bool CanBeSelected(const FVector2D& MousePositionInNode) const override;

	/** タイトルバーのサイズを返す */
	virtual FVector2D GetDesiredSizeForMarquee() const override;

	/** タイトルバーの FSlateRect を返す */
	virtual FSlateRect GetTitleRect() const override;

protected:
	
	virtual ECustomizableCommentNodeZone FindMouseZone(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) const;

	virtual ECustomizableCommentNodeZone FindMouseZone(const FVector2D& LocalMouseCoordinates) const;

	/** @return 現在のウィンドウゾーンが選択領域とみなされる場合 true */
	bool InSelectionArea() const;

	/** @return 通過したゾーンが選択領域である場合 true */
	static bool InSelectionArea(const ECustomizableCommentNodeZone InMouseZone);

	void UpdateResizeAnchorPoint();

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

	void HandleSelection(const bool bInIsSelected, const bool bUpdateNodesUnderComment = false) const;

	void UpdateNodesUnderComment() const;
	
	/** ノードがこのコメントウィジェットの下にあるかどうかを判定するヘルパー関数 */
	virtual bool IsNodeUnderComment(UEdGraphNode_Comment* InCommentNode, const TSharedRef<SGraphNode> InNodeWidget) const;

	/** ユーザーがコメントノードを移動するときに呼び出される */
	virtual void MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty = true) override;

	/** @return コメント本文の色 */
	FSlateColor GetCommentBodyColor() const;

	/** @return タイトルバーの色 */
	FSlateColor GetCommentTitleBarColor() const;

	/** @return コメントバブルの色 */
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

	ECustomizableCommentNodeZone MouseLocatedZone;

	bool bUserIsDragging;
	
	/** キャッシュされたコメントタイトル */
	FString CachedCommentTitle;

	/** キャッシュされたフォントサイズ */
	int32 CachedFontSize;

	/** 最後のフレームでバブルが見えることが望まれていたのか？ */
	mutable bool bCachedBubbleVisibility;

private:
	
	/** コメントのテキストを折り返す幅を返す */
	float GetWrapAt() const;

	/** ズームアウト時に使用するコメントバブルウィジェット */
	TSharedPtr<SCommentBubble> CommentBubble;

	/** コメントの現在の選択状態 */
	mutable bool bIsSelected;

	/** タイトルバーの高さを取得するために必要なタイトルバー */
	TSharedPtr<SBorder> TitleBarBorder;

	/** キャッシュされたコメントタイトルの幅 */
	int32 CachedWidth;

	/** コメントスタイルのローカルコピー */
	FInlineEditableTextBlockStyle CommentStyle;
};
