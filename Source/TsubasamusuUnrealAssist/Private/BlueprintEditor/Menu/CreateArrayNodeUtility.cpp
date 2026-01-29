// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "BlueprintEditor/Menu/CreateArrayNodeUtility.h"
#include  "BlueprintEditor/GraphNodeUtility.h"
#include "K2Node_MakeArray.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "FCreateArrayNodeUtility"

void FCreateArrayNodeUtility::AddCreateArrayNodeMenu(const TWeakObjectPtr<UEdGraph> InGraph, FMenuBuilder& InMenuBuilder, const TSharedPtr<const FEdGraphPinType> ArrayNodePinType)
{
	const FText LabelText = LOCTEXT("CreateArrayNodeLabelText", "Make Array");
	const FText ToolTipText = LOCTEXT("CreateArrayNodeToolTipText", "Make an array with all selected nodes connected.");

	const FSlateIcon MenuIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.MakeArray_16x");

	InMenuBuilder.AddMenuEntry(LabelText, ToolTipText, MenuIcon, FUIAction(FExecuteAction::CreateLambda([InGraph, ArrayNodePinType]()
	{
		if (InGraph.IsValid())
		{
			const TArray<UEdGraphNode*> SelectedNodes = FGraphNodeUtility::GetSelectedNodes(InGraph.Get());
		
			CreateArrayNode(InGraph, SelectedNodes, *ArrayNodePinType);
		}
	})));
}

UK2Node_MakeArray* FCreateArrayNodeUtility::CreateArrayNode(const TWeakObjectPtr<UEdGraph> InGraph, const TArray<UEdGraphNode*>& InNodes, const FEdGraphPinType& ArrayNodePinType)
{
	if (!InGraph.IsValid())
	{
		return nullptr;
	}
	
	const FText TransactionSessionName = LOCTEXT("CreateArrayNodeTransaction", "Create Array Node");
	FScopedTransaction Transaction(TransactionSessionName);

	UEdGraph* Graph = InGraph.Get();
	
	Graph->Modify();

	UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(Graph);
	
	if (IsValid(Blueprint))
	{
		Blueprint->Modify();
	}
	
	TArray<UEdGraphPin*> NodesOutputPins = FGraphNodeUtility::GetNodesOutputPins(InNodes, ArrayNodePinType);
	FGraphNodeUtility::SortPinsByPositionY(NodesOutputPins);
	
	FGraphNodeCreator<UK2Node_MakeArray> ArrayNodeCreator(*Graph);
	
	UK2Node_MakeArray* CreatedArrayNode = ArrayNodeCreator.CreateNode();
	
	if (!IsValid(CreatedArrayNode))
	{
		return nullptr;
	}
	
	CreatedArrayNode->AllocateDefaultPins();

	const FIntPoint DesiredArrayNodePosition = GetDesiredArrayNodePosition(InNodes);
	CreatedArrayNode->NodePosX = DesiredArrayNodePosition.X;
	CreatedArrayNode->NodePosY = DesiredArrayNodePosition.Y;

	for (int32 i = 0; i < NodesOutputPins.Num() - 1; i++)
	{
		CreatedArrayNode->AddInputPin();
	}

	ArrayNodeCreator.Finalize();
	
	const TArray<UEdGraphPin*> CreatedArrayNodeInputPins = FGraphNodeUtility::GetNodeInputPins(CreatedArrayNode);

	for (UEdGraphPin* NodesOutputPin : NodesOutputPins)
	{
		if (!NodesOutputPin)
		{
			continue;
		}

		for (UEdGraphPin* CreatedArrayNodeInputPin : CreatedArrayNodeInputPins)
		{
			if (!CreatedArrayNodeInputPin || CreatedArrayNodeInputPin->HasAnyConnections())
			{
				continue;
			}

			if (Graph->GetSchema()->TryCreateConnection(CreatedArrayNodeInputPin, NodesOutputPin))
			{
				break;
			}
		}
	}
	
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

	return CreatedArrayNode;
}

FIntPoint FCreateArrayNodeUtility::GetDesiredArrayNodePosition(const TArray<UEdGraphNode*>& InNodes)
{
	const int32 DesiredArrayNodePositionX = GetNodesMaxPositionX(InNodes) + 200;
	const int32 DesiredArrayNodePositionY = GetNodesAveragePositionY(InNodes);

	return FIntPoint(DesiredArrayNodePositionX, DesiredArrayNodePositionY);
}

int32 FCreateArrayNodeUtility::GetNodesMaxPositionX(const TArray<UEdGraphNode*>& InNodes)
{
	int32 MaxPositionX = 0;

	for (const UEdGraphNode* Node : InNodes)
	{
		if (!IsValid(Node))
		{
			continue;
		}
		
		if (MaxPositionX == 0)
		{
			MaxPositionX = Node->NodePosX;

			continue;
		}

		if (Node->NodePosX > MaxPositionX)
		{
			MaxPositionX = Node->NodePosX;
		}
	}

	return MaxPositionX;
}

int32 FCreateArrayNodeUtility::GetNodesAveragePositionY(const TArray<UEdGraphNode*>& InNodes)
{
	int32 TotalPositionY = 0;
	
	for (const UEdGraphNode* Node : InNodes)
	{
		if (IsValid(Node))
		{
			TotalPositionY += Node->NodePosY;
		}
	}

	return TotalPositionY / InNodes.Num();
}

#undef LOCTEXT_NAMESPACE
