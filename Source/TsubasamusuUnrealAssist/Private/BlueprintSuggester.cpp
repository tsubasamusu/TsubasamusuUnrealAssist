// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "BlueprintSuggester.h"
#include "GraphNodeUtility.h"
#include "Framework/Docking/TabManager.h"
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
