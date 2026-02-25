// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

struct FGraphActionNode;

class FNodePreviewer final : public FGCObject
{
public:
	void TryPreviewNode();
	
	//~ Begin FGCObject Interface
	virtual void AddReferencedObjects(FReferenceCollector& InReferenceCollector) override;
	virtual FString GetReferencerName() const override
	{
		return TEXT("FNodePreviewer");
	}
	//~ End FGCObject Interface
	
private:
	static TSharedPtr<SWidget> GetHoveredWidget();
	static TSharedPtr<FGraphActionNode> GetGraphActionNodeFromWidget(const TSharedPtr<SWidget> InWidget);
	
	static bool IsNodeSelectionWidget(const TSharedPtr<SWidget> InWidget);
	
	static TSharedPtr<SGraphNode> CreateNodeWidget(UEdGraphNode* InNode);
	UEdGraphNode* CreateNodeFromGraphActionNode(const TSharedPtr<FGraphActionNode> InGraphActionNode);
	
	static TSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>> FindParentNodeTreeViewFromWidget(const TSharedPtr<SWidget> InWidget);
	static TSharedPtr<SWidget> FindWidgetDisplayingToolTipFromNodeSelectionWidget(const TSharedPtr<SWidget> InNodeSelectionWidget);
	static TSharedPtr<SInlineEditableTextBlock> FindParentInlineEditableTextBlock(const TSharedPtr<SWidget> InWidget);
	
	template<typename WidgetClass>
	static TSharedPtr<WidgetClass> FindParentWidget(const TSharedPtr<SWidget> InWidget, const FName& WidgetTypeToFind)
	{
		static_assert(TIsDerivedFrom<WidgetClass, SWidget>::Value, "WidgetClass must be derived from SWidget.");

		for (TSharedPtr<SWidget> Widget = InWidget; Widget.IsValid(); Widget = Widget->GetParentWidget())
		{
			if (Widget->GetType() == WidgetTypeToFind)
			{
				return StaticCastSharedPtr<WidgetClass>(Widget);
			}
		}

		return nullptr;
	}
	
	TWeakPtr<SWidget> WidgetWhoseToolTipWasPreviouslyEdited;
	TWeakPtr<SToolTip> LastBeforeEditingToolTipWidget;
	
	TObjectPtr<UBlueprint> CachedTemporaryBlueprint;
	TObjectPtr<UEdGraph> CachedTemporaryGraph;
};
