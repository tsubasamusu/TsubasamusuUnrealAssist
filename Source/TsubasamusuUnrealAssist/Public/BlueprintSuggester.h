// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class TSUBASAMUSUUNREALASSIST_API FBlueprintSuggester final : public TSharedFromThis<FBlueprintSuggester>
{
public:
	
	void OnNodeAdded(UEdGraphNode* AddedNode);

private:

	void OnNodesAdded(TArray<TWeakObjectPtr<UEdGraphNode>> WeakAddedNodes);

	TArray<TWeakObjectPtr<UEdGraphNode>> CachedWeakAddedNodes;

	FTSTicker::FDelegateHandle WaitSuggestionHandle;
	
	bool bIsWaitingSuggestion;
	
	float CurrentWaitSuggestionSeconds;
	const float MaxWaitSuggestionSeconds = 1.0f;
};
