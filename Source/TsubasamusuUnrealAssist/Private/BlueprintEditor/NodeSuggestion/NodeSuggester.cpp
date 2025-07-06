// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "BlueprintEditor/NodeSuggestion/NodeSuggester.h"
#include "BlueprintActionMenuBuilder.h"
#include "BlueprintActionMenuUtils.h"
#include "BlueprintEditor.h"
#include "BlueprintEditorSettings.h"
#include "BlueprintEditor/NodeSuggestion/ContextMenuTargetProfile.h"
#include  "BlueprintEditor/GraphNodeUtility.h"
#include "SGraphActionMenu.h"
#include "SGraphPanel.h"
#include "SMyBlueprint.h"
#include "SSubobjectEditor.h"
#include "BlueprintEditor/NodeSuggestion/TsubasamusuBlueprintAction_PromoteVariable.h"
#include "BlueprintEditor/TsubasamusuBlueprintEditor.h"
#include "Framework/Docking/TabManager.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Toolkits/AssetEditorToolkit.h"

void FNodeSuggester::OnNodeAdded(UEdGraphNode* AddedNode)
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
	
	const TWeakPtr<FNodeSuggester> WeakNodeSuggester = SharedThis(this);

	WaitSuggestionHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([WeakNodeSuggester](const float DeltaTime)
	{
		if (!WeakNodeSuggester.IsValid())
		{
			return false;
		}

		const TSharedPtr<FNodeSuggester> PinnedNodeSuggester = WeakNodeSuggester.Pin();
		
		PinnedNodeSuggester->CurrentWaitSuggestionSeconds += DeltaTime;

		if (PinnedNodeSuggester->CurrentWaitSuggestionSeconds < PinnedNodeSuggester->MaxWaitSuggestionSeconds)
		{
			return true;
		}

		FTSTicker::GetCoreTicker().RemoveTicker(PinnedNodeSuggester->WaitSuggestionHandle);
		PinnedNodeSuggester->bIsWaitingSuggestion = false;
		PinnedNodeSuggester->CurrentWaitSuggestionSeconds = 0.0f;

		FGraphNodeUtility::RemoveInvalidWeakNodes(PinnedNodeSuggester->CachedWeakAddedNodes);

		if (PinnedNodeSuggester->CachedWeakAddedNodes.Num() > 0)
		{
			UEdGraph* Graph = PinnedNodeSuggester->CachedWeakAddedNodes[0].Get()->GetGraph();
			
			if (IsValid(Graph))
			{
				PinnedNodeSuggester->OnNodesAdded(Graph, PinnedNodeSuggester->CachedWeakAddedNodes);
			}
		}
		
		PinnedNodeSuggester->CachedWeakAddedNodes.Empty();

		return false;
	}));

	bIsWaitingSuggestion = true;
}

void FNodeSuggester::OnNodesAdded(UEdGraph* InGraph, TArray<TWeakObjectPtr<UEdGraphNode>> WeakAddedNodes)
{
	//TODO: suggest nodes
}

TSharedPtr<SGraphActionMenu> FNodeSuggester::CreateGraphActionMenu(const TSharedPtr<FTsubasamusuBlueprintEditor> TsubasamusuBlueprintEditor, const FVector2f& CreatePosition, const FVector2f& AddNodePosition, UEdGraphNode* ForNode, UEdGraphPin* ForPin, const TArray<UEdGraphPin*>& DragFromPins)
{
	check(TsubasamusuBlueprintEditor.IsValid());
	
	const TWeakPtr<SGraphEditor> GraphEditor = TsubasamusuBlueprintEditor->GetFocusedGraphEditor();
	check(GraphEditor.IsValid());

	SGraphPanel* GraphPanel = GraphEditor.Pin()->GetGraphPanel();
	check(GraphPanel);

	const TSharedPtr<SWidget> CreatedWidget = GraphPanel->SummonContextMenu(CreatePosition, AddNodePosition, ForNode, ForPin, DragFromPins);
	return StaticCastSharedPtr<SGraphActionMenu>(CreatedWidget);
}

TSharedPtr<FTsubasamusuBlueprintEditor> FNodeSuggester::GetTsubasamusuBlueprintEditor(const UEdGraph* InGraph)
{
	const TSharedPtr<IBlueprintEditor> BlueprintEditor = FKismetEditorUtilities::GetIBlueprintEditorForObject(InGraph, false);
	return StaticCastSharedPtr<FTsubasamusuBlueprintEditor>(BlueprintEditor);
}

void FNodeSuggester::ConstructActionContext(FBlueprintActionContext& OutBlueprintActionContext, UEdGraph* InGraph, TSharedPtr<FTsubasamusuBlueprintEditor> TsubasamusuBlueprintEditor, const TArray<UEdGraphPin*>& DragFromPins)
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

void FNodeSuggester::TryInsertPromoteToVariable(const FBlueprintActionContext& BlueprintActionContext, FGraphActionListBuilderBase& OutGraphActionListBuilderBase, const UEdGraph* InGraph, TSharedPtr<FTsubasamusuBlueprintEditor> TsubasamusuBlueprintEditor)
{
	const UEdGraphSchema_K2* K2Schema = Cast<const UEdGraphSchema_K2>(InGraph->GetSchema());
	
	if (!K2Schema || BlueprintActionContext.Pins.Num() == 0)
	{
		return;
	}
	
	if (K2Schema->CanPromotePinToVariable(*BlueprintActionContext.Pins[0], true))
	{
		const TSharedPtr<FTsubasamusuBlueprintAction_PromoteVariable> PromoteMemberVariableAction = MakeShared<FTsubasamusuBlueprintAction_PromoteVariable>(true);
		PromoteMemberVariableAction->MyBlueprintEditor = TsubasamusuBlueprintEditor;
		OutGraphActionListBuilderBase.AddAction(PromoteMemberVariableAction);
	}

	if (BlueprintActionContext.Graphs.Num() == 1 && FBlueprintEditorUtils::DoesSupportLocalVariables(BlueprintActionContext.Graphs[0]) && K2Schema->CanPromotePinToVariable(*BlueprintActionContext.Pins[0], false))
	{
		const TSharedPtr<FTsubasamusuBlueprintAction_PromoteVariable> PromoteLocalVariableAction = MakeShared<FTsubasamusuBlueprintAction_PromoteVariable>(false);
		PromoteLocalVariableAction->MyBlueprintEditor = TsubasamusuBlueprintEditor;
		OutGraphActionListBuilderBase.AddAction(PromoteLocalVariableAction);
	}
}

TSharedRef<FGraphActionListBuilderBase> FNodeSuggester::GetGraphActionListBuilderBase(UEdGraph* InGraph, TSharedPtr<FTsubasamusuBlueprintEditor> TsubasamusuBlueprintEditor, const TArray<UEdGraphPin*>& DragFromPins)
{
	check(TsubasamusuBlueprintEditor.IsValid());
	bool const bIsContextSensitive = TsubasamusuBlueprintEditor->GetIsContextSensitive();

	FBlueprintActionContext BlueprintActionContext;
	ConstructActionContext(BlueprintActionContext, InGraph, TsubasamusuBlueprintEditor, DragFromPins);
	
	//TODO: get ContextTargetMask
	uint32 ContextTargetMask = 0;
	//const FContextMenuTargetProfile ContextMenuTargetProfile = FContextMenuTargetProfile(BlueprintActionContext);
	//uint32 ContextTargetMask = ContextMenuTargetProfile.GetContextTargetMask();

	FBlueprintActionMenuBuilder::EConfigFlags ConfigFlags = FBlueprintActionMenuBuilder::DefaultConfig;

	if (GetDefault<UBlueprintEditorSettings>()->bEnableContextMenuTimeSlicing)
	{
		ConfigFlags |= FBlueprintActionMenuBuilder::UseTimeSlicing;
	}
	
	const TSharedPtr<FBlueprintActionMenuBuilder> BlueprintActionMenuBuilder = MakeShared<FBlueprintActionMenuBuilder>(ConfigFlags);
	
	if (!GIsSavingPackage && !IsGarbageCollecting() && BlueprintActionContext.Blueprints.Num() > 0)
	{
		FBlueprintActionMenuUtils::MakeContextMenu(BlueprintActionContext, bIsContextSensitive, ContextTargetMask, *BlueprintActionMenuBuilder);
	}
	
	TryInsertPromoteToVariable(BlueprintActionContext, *BlueprintActionMenuBuilder, InGraph, TsubasamusuBlueprintEditor);
	
	if (const UEdGraphSchema* Schema = Cast<const UEdGraphSchema>(InGraph->GetSchema()))
	{
		Schema->InsertAdditionalActions(BlueprintActionContext.Blueprints, BlueprintActionContext.Graphs, BlueprintActionContext.Pins, *BlueprintActionMenuBuilder);
	}
	
	return BlueprintActionMenuBuilder.ToSharedRef();
}
