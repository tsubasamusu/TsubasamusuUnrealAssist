// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "TickableEditorSubsystem.h"
#include "NodePreviewer.generated.h"

struct FGraphActionNode;

UCLASS()
class UNodePreviewer final : public UTickableEditorSubsystem
{
	GENERATED_BODY()
	
public:
	//~ Begin USubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& InSubsystemCollectionBase) override;
	//~ End USubsystem Interface
	
protected:
	//~ Begin UTickableEditorSubsystem Interface
	virtual void Tick(const float InDeltaTime) override;
	//~ End UTickableEditorSubsystem Interface
	
private:
	void TryPreviewNode();
	
	static TSharedPtr<SWidget> GetHoveredWidget();
	static TSharedPtr<FGraphActionNode> GetGraphActionNode(const TSharedPtr<SWidget> InWidget);
	
	static TSharedPtr<SGraphNode> CreateNodeWidget(UEdGraphNode* InNode);
	static const UBlueprintNodeSpawner* GetBlueprintNodeSpawner(const TSharedPtr<FGraphActionNode> InGraphActionNode);
	
	static bool IsDescendantOfBlueprintPaletteItem(const TSharedPtr<SWidget> InWidget);
	static TSharedPtr<SWidget> FindToolTipDisplayer(const TSharedPtr<SWidget> InNodeSelectionWidget);
	static UEdGraph* GetFocusedGraph();
	
	UPROPERTY()
	TObjectPtr<UBlueprint> GhostBlueprint;
	
	UPROPERTY()
	TObjectPtr<UEdGraph> GhostGraph;
	
	/* A widget whose tool tip was previously edited. */
	TWeakPtr<SWidget> ToolTipEditedWidget;
	
	/* Previously edited tool tip widget. */
	TWeakPtr<SToolTip> EditedToolTipWidget;
	
	TWeakObjectPtr<UEdGraphNode> LastCreatedNode;
};
