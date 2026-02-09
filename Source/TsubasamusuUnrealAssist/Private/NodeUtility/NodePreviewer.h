// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class SDocumentationToolTip;
struct FGraphActionNode;

class FNodePreviewer final : public FGCObject
{
public:
	void TryPreviewNode();
	
	//~ Begin FGCObject Interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override
	{
		return TEXT("FNodePreviewer");
	}
	//~ End FGCObject Interface
	
private:
	static TSharedPtr<SWidget> GetHoveredWidget();
	
	static bool IsNodeSelectionWidget(const TSharedPtr<SWidget> InWidget);
	
	static TSharedPtr<FGraphActionNode> GetGraphActionNodeFromWidget(const TSharedPtr<SWidget> InWidget);
	static TSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>> GetNodeTreeViewFromWidget(const TSharedPtr<SWidget> InWidget);
	
	static TSharedPtr<SGraphNode> CreateNodeWidget(UEdGraphNode* InNode);
	UEdGraphNode* CreateNodeFromGraphActionNode(const TSharedPtr<FGraphActionNode> InGraphActionNode);
	
	static TSharedPtr<SWidget> FindWidgetDisplayingToolTipFromNodeSelectionWidget(const TSharedPtr<SWidget> InNodeSelectionWidget);
	static TSharedPtr<SInlineEditableTextBlock> FindParentInlineEditableTextBlock(const TSharedPtr<SWidget> InWidget);
	
	TWeakPtr<SWidget> WidgetWhoseToolTipWasPreviouslyEdited;
	TWeakPtr<SToolTip> LastBeforeEditingToolTipWidget;
	
	TObjectPtr<UBlueprint> CachedTemporaryBlueprint;
	TObjectPtr<UEdGraph> CachedTemporaryGraph;
};
