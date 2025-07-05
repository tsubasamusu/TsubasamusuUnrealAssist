// Copyright (c) 2025, tsubasamusu All rights reserved.

#include  "GraphNodeUtility.h"

#include "Debug/TsubasamusuLogUtility.h"

TArray<UEdGraphNode*> FGraphNodeUtility::GetSelectedNodes(const UEdGraph* InGraph)
{
	TArray<UEdGraphNode*> SelectedNodes;

	if (!IsValid(InGraph))
	{
		return SelectedNodes;
	}
	
	const TSharedPtr<SGraphEditor> GraphEditor = SGraphEditor::FindGraphEditorForGraph(InGraph);

	if (!GraphEditor.IsValid())
	{
		return SelectedNodes;
	}

	const FGraphPanelSelectionSet& SelectedNodesSet = GraphEditor->GetSelectedNodes();

	for (UObject* SelectedObject : SelectedNodesSet)
	{
		UEdGraphNode* SelectedNode = Cast<UEdGraphNode>(SelectedObject);
		
		if (IsValid(SelectedNode))
		{
			SelectedNodes.Add(SelectedNode);
		}
	}
	
	return SelectedNodes;
}

bool FGraphNodeUtility::TryGetNodesOutputPinsCommonType(const TArray<UEdGraphNode*>& InNodes, FEdGraphPinType& OutNodesOutputPinsCommonType)
{
	const TArray<UEdGraphPin*> NodesOutputPins = GetNodesOutputPins(InNodes);
	
	for (const UEdGraphPin* NodesOutputPin : NodesOutputPins)
	{
		int32 HavingSameTypeOutputPinsNodeCount = 0;
		
		for (const UEdGraphNode* Node : InNodes)
		{
			if (NodeHasSameTypeOutputPins(Node, NodesOutputPin->PinType))
			{
				HavingSameTypeOutputPinsNodeCount++;
			}
		}

		if (HavingSameTypeOutputPinsNodeCount == InNodes.Num())
		{
			OutNodesOutputPinsCommonType = NodesOutputPin->PinType;
			
			return true;
		}
	}

	return false;
}

bool FGraphNodeUtility::NodeHasSameTypeOutputPins(const UEdGraphNode* InNode, const FEdGraphPinType& InPinType)
{
	if (!IsValid(InNode))
	{
		return false;
	}
	
	const TArray<UEdGraphPin*>& NodePins = InNode->GetAllPins();

	for (const UEdGraphPin* NodePin : NodePins)
	{
		if (NodePin->PinType == InPinType)
		{
			return true;
		}
	}

	return false;
}

TArray<UEdGraphPin*> FGraphNodeUtility::GetNodesOutputPins(const TArray<UEdGraphNode*>& InNodes)
{
	TArray<UEdGraphPin*> OutputPins;

	for (const UEdGraphNode* Node : InNodes)
	{
		if (!IsValid(Node))
		{
			continue;
		}
		
		const TArray<UEdGraphPin*>& NodePins = Node->GetAllPins();

		for (UEdGraphPin* NodePin : NodePins)
		{
			if (NodePin->Direction == EGPD_Output && !NodePin->GetDisplayName().IsEmpty())
			{
				OutputPins.Add(NodePin);
			}
		}
	}

	return OutputPins;
}

TArray<UEdGraphPin*> FGraphNodeUtility::GetNodesOutputPins(const TArray<UEdGraphNode*>& InNodes, const FEdGraphPinType& SpecificPinType)
{
	const TArray<UEdGraphPin*> NodesOutputPins = GetNodesOutputPins(InNodes);
	TArray<UEdGraphPin*> SpecificTypeOutputPins;

	for (UEdGraphPin* NodeOutputPin : NodesOutputPins)
	{
		if (NodeOutputPin && NodeOutputPin->PinType == SpecificPinType)
		{
			SpecificTypeOutputPins.Add(NodeOutputPin);
		}
	}

	return SpecificTypeOutputPins;
}

TArray<UEdGraphPin*> FGraphNodeUtility::GetNodeInputPins(const UEdGraphNode* InNode)
{
	TArray<UEdGraphPin*> NodeInputPins;

	if (!IsValid(InNode))
	{
		return NodeInputPins;
	}
	
	const TArray<UEdGraphPin*>& NodePins = InNode->GetAllPins();

	for (UEdGraphPin* NodePin : NodePins)
	{
		if (NodePin->Direction == EGPD_Input)
		{
			NodeInputPins.Add(NodePin);
		}
	}

	return NodeInputPins;
}

void FGraphNodeUtility::SortPinsByPositionY(TArray<UEdGraphPin*>& OutPins)
{
	TArray<int32> OutPinsIndexes;
	OutPinsIndexes.SetNum(OutPins.Num());

	for (int32 i = 0; i < OutPinsIndexes.Num(); i++)
	{
		OutPinsIndexes[i] = i;
	}

	OutPinsIndexes.StableSort([&OutPins](const int32 FirstIndex, const int32 SecondIndex)
	{
		const int32 FirstPinPositionY = GetPinPosition(OutPins[FirstIndex]).Y;
		const int32 SecondPinPositionY = GetPinPosition(OutPins[SecondIndex]).Y;
		
		return FirstPinPositionY < SecondPinPositionY;
	});

	TArray<UEdGraphPin*> SortedPins;
	SortedPins.SetNum(OutPins.Num());
	
	for (int32 i = 0; i < SortedPins.Num(); i++)
	{
		SortedPins[i] = OutPins[OutPinsIndexes[i]];
	}
	
	OutPins = SortedPins;
}

FIntPoint FGraphNodeUtility::GetPinPosition(const UEdGraphPin* InPin)
{
	if (!InPin)
	{
		return FIntPoint();
	}

	const UEdGraphNode* PinNode = InPin->GetOwningNode();

	if (!IsValid(PinNode))
	{
		return FIntPoint();
	}

	return FIntPoint(PinNode->NodePosX, PinNode->NodePosY);
}

TArray<UEdGraphNode*> FGraphNodeUtility::ConvertToHardNodes(const TArray<TWeakObjectPtr<UEdGraphNode>>& InWeakNodes)
{
	TArray<UEdGraphNode*> Nodes;

	for (const TWeakObjectPtr<UEdGraphNode>& WeakNode : InWeakNodes)
	{
		check(WeakNode.IsValid());
		
		if (WeakNode.IsValid())
		{
			Nodes.Add(WeakNode.Get());
		}
	}

	return Nodes;
}

void FGraphNodeUtility::RemoveInvalidWeakNodes(TArray<TWeakObjectPtr<UEdGraphNode>>& OutWeakNodes)
{
	for (TWeakObjectPtr<UEdGraphNode> WeakNode : OutWeakNodes)
	{
		if (!WeakNode.IsValid())
		{
			OutWeakNodes.Remove(WeakNode);
		}
	}
}
