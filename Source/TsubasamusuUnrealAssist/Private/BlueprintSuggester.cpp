// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "BlueprintSuggester.h"
#include "BlueprintEditor.h"
#include "GraphNodeUtility.h"
#include "SGraphActionMenu.h"
#include "SGraphPanel.h"
#include "SMyBlueprint.h"
#include "SSubobjectEditor.h"
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

TSharedPtr<SGraphActionMenu> FBlueprintSuggester::CreateGraphActionMenu(const TSharedPtr<FTsubasamusuBlueprintEditor> TsubasamusuBlueprintEditor, const FVector2f& CreatePosition, const FVector2f& AddNodePosition, UEdGraphNode* ForNode, UEdGraphPin* ForPin, const TArray<UEdGraphPin*>& DragFromPins)
{
	check(TsubasamusuBlueprintEditor.IsValid());
	
	const TWeakPtr<SGraphEditor> GraphEditor = TsubasamusuBlueprintEditor->GetFocusedGraphEditor();
	check(GraphEditor.IsValid());

	SGraphPanel* GraphPanel = GraphEditor.Pin()->GetGraphPanel();
	check(GraphPanel);

	const TSharedPtr<SWidget> CreatedWidget = GraphPanel->SummonContextMenu(CreatePosition, AddNodePosition, ForNode, ForPin, DragFromPins);
	return StaticCastSharedPtr<SGraphActionMenu>(CreatedWidget);
}

TSharedPtr<FTsubasamusuBlueprintEditor> FBlueprintSuggester::GetTsubasamusuBlueprintEditor(const UEdGraph* InGraph)
{
	const TSharedPtr<IBlueprintEditor> BlueprintEditor = FKismetEditorUtilities::GetIBlueprintEditorForObject(InGraph, false);
	return StaticCastSharedPtr<FTsubasamusuBlueprintEditor>(BlueprintEditor);
}

void FBlueprintSuggester::ConstructActionContext(FBlueprintActionContext& OutBlueprintActionContext, UEdGraph* InGraph, TSharedPtr<FTsubasamusuBlueprintEditor> TsubasamusuBlueprintEditor, const TArray<UEdGraphPin*>& DragFromPins)
{
	check(TsubasamusuBlueprintEditor.IsValid());
	bool const bIsContextSensitive = TsubasamusuBlueprintEditor->GetIsContextSensitive();
	
	OutBlueprintActionContext.Graphs.Add(InGraph);
	
	UBlueprint* Blueprint = TsubasamusuBlueprintEditor->GetBlueprintObj();
	const bool bBlueprintIsValid = IsValid(Blueprint) && Blueprint->GeneratedClass && Blueprint->GeneratedClass->ClassGeneratedBy == Blueprint;

	if (!ensure(bBlueprintIsValid))
	{
		return;
	}
	
	OutBlueprintActionContext.EditorPtr = TsubasamusuBlueprintEditor;
	OutBlueprintActionContext.Blueprints.Add(Blueprint);

	if (!bIsContextSensitive)
	{
		return;
	}
	
	OutBlueprintActionContext.Pins = DragFromPins;
	
	const FEdGraphSchemaAction_K2Var* SelectedVar = TsubasamusuBlueprintEditor->GetMyBlueprintWidget()->SelectionAsVar();

	if (SelectedVar != nullptr && SelectedVar->GetProperty() != nullptr)
	{
		OutBlueprintActionContext.SelectedObjects.Add(SelectedVar->GetProperty());

		return;
	}
	
	if (!Blueprint->SkeletonGeneratedClass || !TsubasamusuBlueprintEditor->GetSubobjectEditor().IsValid())
	{
		return;
	}
	
	TArray<FSubobjectEditorTreeNodePtrType> Nodes = TsubasamusuBlueprintEditor->GetSubobjectEditor()->GetSelectedNodes();

	if (Nodes.Num() == 1 && Nodes[0]->IsComponentNode())
	{
		const FName PropertyName = Nodes[0]->GetVariableName();
		const FObjectProperty* VariableProperty = FindFProperty<FObjectProperty>(Blueprint->SkeletonGeneratedClass, PropertyName);
		OutBlueprintActionContext.SelectedObjects.Add(VariableProperty);
	}
}
