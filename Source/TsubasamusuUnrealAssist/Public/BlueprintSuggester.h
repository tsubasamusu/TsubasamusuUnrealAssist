// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class SGraphActionMenu;

class TSUBASAMUSUUNREALASSIST_API FBlueprintSuggester final : public TSharedFromThis<FBlueprintSuggester>
{
public:
	
	void OnNodeAdded(UEdGraphNode* AddedNode);

private:

	void OnNodesAdded(UEdGraph* InGraph, TArray<TWeakObjectPtr<UEdGraphNode>> WeakAddedNodes);

	static TSharedPtr<SGraphActionMenu> CreateGraphActionMenu(const UEdGraph* InGraph, const FVector2f& CreatePosition, const FVector2f& AddNodePosition, UEdGraphNode* ForNode = nullptr, UEdGraphPin* ForPin = nullptr, const TArray<UEdGraphPin*>& DragFromPins = TArray<UEdGraphPin*>());
	
	TArray<TWeakObjectPtr<UEdGraphNode>> CachedWeakAddedNodes;

	FTSTicker::FDelegateHandle WaitSuggestionHandle;
	
	bool bIsWaitingSuggestion;
	
	float CurrentWaitSuggestionSeconds;
	const float MaxWaitSuggestionSeconds = 1.0f;
};
