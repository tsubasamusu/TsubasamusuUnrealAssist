// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "BlueprintSuggester.h"
#include "BlueprintEditor.h"
#include "GraphNodeUtility.h"
#include "SGraphActionMenu.h"
#include "SGraphPanel.h"
#include "TsubasamusuBlueprintEditor.h"
#include "Framework/Docking/TabManager.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Toolkits/AssetEditorToolkit.h"

void FBlueprintSuggester::OnNodeAdded(UEdGraphNode* AddedNode)
{
	if (!IsValid(AddedNode))
	{
		return;
	}
	
	const TWeakObjectPtr<UEdGraphNode> WeakAddedNode = AddedNode;
	CachedWeakAddedNodes.AddUnique(WeakAddedNode);

	if (bIsWaitingSuggestion)
	{
		return;
	}
	
	const TWeakPtr<FBlueprintSuggester> WeakBlueprintSuggester = SharedThis(this);

	WaitSuggestionHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([WeakBlueprintSuggester](const float DeltaTime)
	{
		if (!WeakBlueprintSuggester.IsValid())
		{
			return false;
		}

		const TSharedPtr<FBlueprintSuggester> PinnedBlueprintSuggester = WeakBlueprintSuggester.Pin();
		
		PinnedBlueprintSuggester->CurrentWaitSuggestionSeconds += DeltaTime;

		if (PinnedBlueprintSuggester->CurrentWaitSuggestionSeconds < PinnedBlueprintSuggester->MaxWaitSuggestionSeconds)
		{
			return true;
		}

		FTSTicker::GetCoreTicker().RemoveTicker(PinnedBlueprintSuggester->WaitSuggestionHandle);
		PinnedBlueprintSuggester->bIsWaitingSuggestion = false;
		PinnedBlueprintSuggester->CurrentWaitSuggestionSeconds = 0.0f;

		FGraphNodeUtility::RemoveInvalidWeakNodes(PinnedBlueprintSuggester->CachedWeakAddedNodes);

		if (PinnedBlueprintSuggester->CachedWeakAddedNodes.Num() > 0)
		{
			UEdGraph* Graph = PinnedBlueprintSuggester->CachedWeakAddedNodes[0].Get()->GetGraph();
			
			if (IsValid(Graph))
			{
				PinnedBlueprintSuggester->OnNodesAdded(Graph, PinnedBlueprintSuggester->CachedWeakAddedNodes);
			}
		}
		
		PinnedBlueprintSuggester->CachedWeakAddedNodes.Empty();

		return false;
	}));

	bIsWaitingSuggestion = true;
}

void FBlueprintSuggester::OnNodesAdded(UEdGraph* InGraph, TArray<TWeakObjectPtr<UEdGraphNode>> WeakAddedNodes)
{
	
}

TSharedPtr<SGraphActionMenu> FBlueprintSuggester::CreateGraphActionMenu(const UEdGraph* InGraph, const FVector2f& CreatePosition, const FVector2f& AddNodePosition, UEdGraphNode* ForNode, UEdGraphPin* ForPin, const TArray<UEdGraphPin*>& DragFromPins)
{
	const TSharedPtr<IBlueprintEditor> BlueprintEditor = FKismetEditorUtilities::GetIBlueprintEditorForObject(InGraph, false);
	const TSharedPtr<FTsubasamusuBlueprintEditor> TsubasamusuBlueprintEditor = StaticCastSharedPtr<FTsubasamusuBlueprintEditor>(BlueprintEditor);
	check(TsubasamusuBlueprintEditor);
	
	const TWeakPtr<SGraphEditor> GraphEditor = TsubasamusuBlueprintEditor->GetFocusedGraphEditor();
	check(GraphEditor.IsValid());

	SGraphPanel* GraphPanel = GraphEditor.Pin()->GetGraphPanel();
	check(GraphPanel);

	const TSharedPtr<SWidget> CreatedWidget = GraphPanel->SummonContextMenu(CreatePosition, AddNodePosition, ForNode, ForPin, DragFromPins);
	return StaticCastSharedPtr<SGraphActionMenu>(CreatedWidget);
}
