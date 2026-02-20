// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "Blueprint/ArrayNodeCreator.h"
#include "Blueprint/NodeUtility.h"
#include "K2Node_MakeArray.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "FArrayNodeCreator"

void FArrayNodeCreator::AddCreateArrayNodeMenu(const TWeakObjectPtr<UEdGraph> InGraph, FMenuBuilder& InMenuBuilder, const TSharedPtr<const FEdGraphPinType> InArrayNodePinType)
{
	const FText LabelText = LOCTEXT("CreateArrayNodeLabel", "Make Array");
	const FText ToolTipText = LOCTEXT("CreateArrayNodeToolTip", "Make an array with all selected nodes connected.");

	const FSlateIcon MenuIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.MakeArray_16x");

	InMenuBuilder.AddMenuEntry(LabelText, ToolTipText, MenuIcon, FUIAction(FExecuteAction::CreateLambda([InGraph, InArrayNodePinType]()
	{
		if (InGraph.IsValid())
		{
			const TArray<UEdGraphNode*> SelectedNodes = FNodeUtility::GetSelectedNodes(InGraph.Get());
		
			CreateArrayNode(InGraph, SelectedNodes, *InArrayNodePinType);
		}
	})));
}

UK2Node_MakeArray* FArrayNodeCreator::CreateArrayNode(const TWeakObjectPtr<UEdGraph> InGraph, const TArray<UEdGraphNode*>& InNodes, const FEdGraphPinType& InArrayNodePinType)
{
	if (!InGraph.IsValid())
	{
		return nullptr;
	}
	
	FScopedTransaction Transaction(LOCTEXT("CreateArrayNodeTransaction", "Create Array Node"));

	UEdGraph* Graph = InGraph.Get();
	
	Graph->Modify();

	UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(Graph);
	
	if (IsValid(Blueprint))
	{
		Blueprint->Modify();
	}
	
	TArray<UEdGraphPin*> NodesOutputPins = FNodeUtility::GetNodesOutputPins(InNodes, InArrayNodePinType);
	FNodeUtility::SortPinsByPositionY(NodesOutputPins);
	
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
	
	const TArray<UEdGraphPin*> CreatedArrayNodeInputPins = FNodeUtility::GetNodeInputPins(CreatedArrayNode);

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

FIntPoint FArrayNodeCreator::GetDesiredArrayNodePosition(const TArray<UEdGraphNode*>& InNodes)
{
	const int32 DesiredArrayNodePositionX = GetNodesMaxPositionX(InNodes) + 200;
	const int32 DesiredArrayNodePositionY = GetNodesAveragePositionY(InNodes);

	return FIntPoint(DesiredArrayNodePositionX, DesiredArrayNodePositionY);
}

int32 FArrayNodeCreator::GetNodesMaxPositionX(const TArray<UEdGraphNode*>& InNodes)
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

int32 FArrayNodeCreator::GetNodesAveragePositionY(const TArray<UEdGraphNode*>& InNodes)
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
