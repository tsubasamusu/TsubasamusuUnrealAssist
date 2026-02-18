// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "NodeUtility/NodeUtility.h"
#include "Editor.h"
#include "EdGraph/EdGraphNode.h"

TArray<UEdGraphNode*> FNodeUtility::GetSelectedNodes(const UEdGraph* InGraph)
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

TArray<TWeakObjectPtr<UEdGraphNode>> FNodeUtility::GetSelectedWeakNodes(const UEdGraph* InGraph)
{
	TArray<UEdGraphNode*> SelectedNodes = GetSelectedNodes(InGraph);
	return ConvertToWeakNodes(SelectedNodes);
}

bool FNodeUtility::TryGetNodesOutputPinsCommonType(const TArray<UEdGraphNode*>& InNodes, FEdGraphPinType& OutNodesOutputPinsCommonType)
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

bool FNodeUtility::NodeHasSameTypeOutputPins(const UEdGraphNode* InNode, const FEdGraphPinType& InPinType)
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

bool FNodeUtility::IsExecPinType(const FEdGraphPinType& InPinType)
{
	return InPinType.PinCategory == UEdGraphSchema_K2::PC_Exec;
}

TArray<UEdGraphPin*> FNodeUtility::GetNodesOutputPins(const TArray<UEdGraphNode*>& InNodes)
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

TArray<UEdGraphPin*> FNodeUtility::GetNodesOutputPins(const TArray<UEdGraphNode*>& InNodes, const FEdGraphPinType& SpecificPinType)
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

TArray<UEdGraphPin*> FNodeUtility::GetNodeInputPins(const UEdGraphNode* InNode)
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

void FNodeUtility::SortPinsByPositionY(TArray<UEdGraphPin*>& OutPins)
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

FIntPoint FNodeUtility::GetPinPosition(const UEdGraphPin* InPin)
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

TArray<UEdGraphNode*> FNodeUtility::ConvertToHardNodes(const TArray<TWeakObjectPtr<UEdGraphNode>>& InWeakNodes)
{
	TArray<UEdGraphNode*> Nodes;

	for (const TWeakObjectPtr<UEdGraphNode> WeakNode : InWeakNodes)
	{
		Nodes.Add(WeakNode.Get());
	}

	return Nodes;
}

TArray<TWeakObjectPtr<UEdGraphNode>> FNodeUtility::ConvertToWeakNodes(const TArray<UEdGraphNode*>& InHardNodes)
{
	TArray<TWeakObjectPtr<UEdGraphNode>> WeakNodes;
	
	for (UEdGraphNode* HardNode : InHardNodes)
	{
		WeakNodes.Add(HardNode);
	}
	
	return WeakNodes;
}

void FNodeUtility::RemoveInvalidWeakNodes(TArray<TWeakObjectPtr<UEdGraphNode>>& OutWeakNodes)
{
	for (TWeakObjectPtr<UEdGraphNode> WeakNode : OutWeakNodes)
	{
		if (!WeakNode.IsValid())
		{
			OutWeakNodes.Remove(WeakNode);
		}
	}
}
