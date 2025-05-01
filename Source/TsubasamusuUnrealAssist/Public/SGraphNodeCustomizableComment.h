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

	//~ Begin SNode Interface
	virtual bool CanBeSelected(const FVector2D& MousePositionInNode) const override;
	virtual FVector2D GetDesiredTitleBarSize() const override;

	//~ Begin SGraphNode Interface
	virtual FSlateRect GetTitleRect() const override;
	//~ End SGraphNode Interface
	
protected:
	
	//~ Begin SGraphNode Interface
	virtual void UpdateGraphNode() override;
	virtual void PopulateMetaTag(class FGraphNodeMetaData* TagMeta) const override;
	//~ End SGraphNode Interface
	
	virtual ECustomizableCommentNodeZone GetMouseZone(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) const;

	virtual ECustomizableCommentNodeZone GetMouseZone(const FVector2D& LocalMouseCoordinates) const;

	bool InSelectionArea() const;

	static bool InSelectionArea(const ECustomizableCommentNodeZone InMouseZone);

	void UpdateAnchorPoint();

	FVector2D GetCorrectedNodePositionByAnchorPoint() const;

	virtual float GetTitleBarHeight() const;

	virtual FVector2D GetNodeMinimumSize() const;

	virtual FVector2D GetNodeMaximumSize() const;

	virtual FSlateRect GetHitTestingBorder() const;

	void HandleSelection(const bool bInIsSelected, const bool bUpdateNodesUnderComment = false) const;

	void UpdateNodesUnderComment() const;

	UObject* GetLastOuter() const;
	
	FVector2D GetDeltaNodeSize(const FVector2D& InDeltaMouseCoordinates) const;

	FVector2D GetSnappedNodeSize() const;

	FVector2D GetDeltaMouseCoordinates(const FPointerEvent& InMouseEvent);

	static FSlateRect GetNodeRect(TSharedRef<const SGraphNode> InNodeWidget);
	
	virtual bool IsNodeUnderComment(TSharedRef<const SGraphNode> InNodeWidget) const;

	virtual void MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty = true) override;

	FSlateColor GetCommentBodyColor() const;

	FSlateColor GetCommentTitleBarColor() const;

	FSlateColor GetCommentBubbleColor() const;

	FVector2D DragSize;

	FVector2D UserSize;

	FVector2D StoredUserSize;

	TSharedPtr<FScopedTransaction> ResizeTransactionPtr;

	FVector2D NodeAnchorPoint;

	ECustomizableCommentNodeZone MouseLocatedZone;

	bool bUserIsDragging;
	
	FString CachedCommentTitle;

	int32 CachedFontSize;

	mutable bool bCachedBubbleVisibility;

private:
	
	float GetWrappingTitleTextWidth() const;

	TSharedPtr<SCommentBubble> CommentBubble;

	mutable bool bIsSelected;

	TSharedPtr<SBorder> TitleBarBorder;

	int32 CachedWidth;

	FInlineEditableTextBlockStyle CommentStyle;
};
