// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

class TSUBASAMUSUUNREALASSIST_API SGraphNodePlayerCharacter : public SGraphNode
{
public:

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
		TitleBar,
	};

	//~ Begin SWidget Interface
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FCursorReply OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const override;
	//~ End SWidget Interface
	
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

protected:

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
};
