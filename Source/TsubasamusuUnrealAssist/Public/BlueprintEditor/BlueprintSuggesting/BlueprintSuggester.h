// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class FTsubasamusuBlueprintEditor;
class SGraphActionMenu;

class TSUBASAMUSUUNREALASSIST_API FBlueprintSuggester final : public TSharedFromThis<FBlueprintSuggester>
{
public:
	
	void OnNodeAdded(UEdGraphNode* AddedNode);

private:

	void OnNodesAdded(UEdGraph* InGraph, TArray<TWeakObjectPtr<UEdGraphNode>> WeakAddedNodes);

	static TSharedPtr<SGraphActionMenu> CreateGraphActionMenu(const TSharedPtr<FTsubasamusuBlueprintEditor> TsubasamusuBlueprintEditor, const FVector2f& CreatePosition, const FVector2f& AddNodePosition, UEdGraphNode* ForNode = nullptr, UEdGraphPin* ForPin = nullptr, const TArray<UEdGraphPin*>& DragFromPins = TArray<UEdGraphPin*>());

	static TSharedPtr<FTsubasamusuBlueprintEditor> GetTsubasamusuBlueprintEditor(const UEdGraph* InGraph);

	static void ConstructActionContext(FBlueprintActionContext& OutBlueprintActionContext, UEdGraph* InGraph, TSharedPtr<FTsubasamusuBlueprintEditor> TsubasamusuBlueprintEditor, const TArray<UEdGraphPin*>& DragFromPins = TArray<UEdGraphPin*>());

	static void TryInsertPromoteToVariable(const FBlueprintActionContext& BlueprintActionContext, FGraphActionListBuilderBase& OutGraphActionListBuilderBase, const UEdGraph* InGraph, TSharedPtr<FTsubasamusuBlueprintEditor> TsubasamusuBlueprintEditor);

	static TSharedRef<FGraphActionListBuilderBase> GetGraphActionListBuilderBase(UEdGraph* InGraph, TSharedPtr<FTsubasamusuBlueprintEditor> TsubasamusuBlueprintEditor, const TArray<UEdGraphPin*>& DragFromPins = TArray<UEdGraphPin*>());
	
	TArray<TWeakObjectPtr<UEdGraphNode>> CachedWeakAddedNodes;

	FTSTicker::FDelegateHandle WaitSuggestionHandle;
	
	bool bIsWaitingSuggestion;
	
	float CurrentWaitSuggestionSeconds;
	const float MaxWaitSuggestionSeconds = 1.0f;
};
