// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

class SCommentBubble;
class UEdGraphNode_Comment;

namespace CustomizableCommentNodeDefinitions
{
	static const FSlateRect HitTestingBorderRect(10, 10, 10, 10);

	static const FVector2f MinNodeSize(30.0f, 30.0f);

	static constexpr float TitleBarColorMultiplier = 0.6f;

	static const FSlateRect TitleBarOffset(13,8,-3,0);

	static const FMargin TitleBarPadding(10, 5, 5, 3);
}

class TSUBASAMUSUUNREALASSIST_API SGraphNodeCustomizableComment : public SGraphNode
{
public:
	
	SLATE_BEGIN_ARGS(SGraphNodeCustomizableComment){}
	SLATE_END_ARGS()

	enum ECommentNodeZone
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
	
	//~ Begin SPanel Interface
	virtual FVector2D ComputeDesiredSize(float) const override;
	//~ End SPanel Interface

	//~ Begin SNodePanel::SNode Interface
	virtual const FSlateBrush* GetShadowBrush(bool bSelected) const override;
	virtual void GetOverlayBrushes(bool bSelected, const FVector2f& WidgetSize, TArray<FOverlayBrushInfo>& Brushes) const override;
	virtual bool ShouldAllowCulling() const override;
	virtual int32 GetSortDepth() const override;
	virtual void EndUserInteraction() const override;
	virtual FString GetNodeComment() const override;
	virtual bool CanBeSelected(const FVector2f& MousePositionInNode) const override;;
	//~ End SNodePanel::SNode Interface

	//~ Begin SGraphNode Interface
	virtual bool IsNameReadOnly() const override;
	virtual FSlateColor GetCommentColor() const override;
	virtual FSlateRect GetTitleRect() const override;
	//~ End SGraphNode Interface

	void Construct(const FArguments& InArgs, UEdGraphNode_Comment* InNode);

protected:
	
	//~ Begin SNodePanel::SNode Interface
	virtual void MoveTo(const FVector2f& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty = true) override;
	//~ End SNodePanel::SNode Interface

	//~ Begin SGraphNode Interface
	virtual void UpdateGraphNode() override;
	virtual void PopulateMetaTag(class FGraphNodeMetaData* TagMeta) const override;
	//~ End SGraphNode Interface
	
	virtual EVisibility GetCommentTextVisibility() const;
	virtual EVisibility GetCommentBubbleVisibility() const;

	virtual EMouseCursor::Type GetTitleBarMouseCursor() const;

	virtual FLinearColor GetTitleBarLineColor() const;
	virtual float GetTitleBarLineOpacity() const;
	
	virtual void CreateCommentNodeWidget(const FGraphNodeMetaData& InGraphNodeMetaData);

	virtual FText GetCommentNodeToolTipText() const;
	
	virtual FSlateColor GetCommentNodeColor() const;
	void SetCommentNodeColor(const FLinearColor& NewCommentNodeColor) const;
	
	void SetCommentNodeAngle(const float NewAngle);
	void SetCommentNodeScale(const float NewScale);

	TSharedPtr<SOverlay> GetTitleBarOverlay() const; 
	TSharedPtr<SOverlay> GetCommentNodeContentOverlay() const; 
	
	float GetTitleBarHeight() const;
	virtual FVector2f GetDesiredTitleBarSize() const;

private:
	
	ECommentNodeZone GetMouseZone(const FGeometry& InGeometry, const FPointerEvent& InPointerEvent) const;
	ECommentNodeZone GetMouseZone(const FVector2f& LocalMouseCoordinates) const;

	FSlateColor GetTitleBarColor() const;
	FSlateColor GetCommentBubbleColor() const;
	
	bool MouseIsInSelectionZone() const;
	static bool IsInSelectionZone(const ECommentNodeZone InCommentNodeZone);

	void UpdateAnchorPoint();

	FVector2f GetCorrectedNodePositionByAnchorPoint() const;
	
	static FVector2f GetMinNodeSize();
	FVector2f GetMaxNodeSize() const;

	static FSlateRect GetHitTestingBorderRect();
	static FSlateRect GetNodeRect(TSharedRef<const SGraphNode> InNodeWidget);
	
	void HandleSelection(const bool bInIsSelected) const;

	void UpdateNodesUnderComment() const;

	UObject* GetLastOuter() const;
	
	FVector2f GetDeltaMouseCoordinates(const FPointerEvent& InPointerEvent);
	FVector2f GetDeltaNodeSize(const FVector2f& InDeltaMouseCoordinates) const;
	FVector2f GetSnappedNodeSize() const;

	bool IsNodeUnderComment(TSharedRef<const SGraphNode> InNodeWidget) const;

	float GetWrappingTitleTextWidth() const;

	void UpdateRenderTransform();
	
	FVector2f DragSize;
	FVector2f UserSize;
	FVector2f StoredUserSize;
	FVector2f NodeAnchorPoint;

	TSharedPtr<FScopedTransaction> ResizeTransactionPtr;
	
	TSharedPtr<SCommentBubble> CommentBubble;
	TSharedPtr<SBorder> TitleBarBorder;
	TSharedPtr<SOverlay> TitleBarOverlay;
	TSharedPtr<SOverlay> CommentNodeContentOverlay;
	
	FString CachedCommentTitle;
	int32 CachedFontSize;
	int32 CachedWidth;
	float CachedCommentNodeAngle = 0.0f;
	float CachedCommentNodeScale = 1.0f;
	mutable bool bCachedBubbleVisibility;
	mutable bool bCachedShiftKeyIsDown;
	
	mutable bool bIsSelected;
	bool bUserIsDragging;
	
	FInlineEditableTextBlockStyle CommentStyle;
	
	ECommentNodeZone MouseLocatedZone;
};
