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

TArray<UEdGraphPin*> FArrayNodeCreator::GetOutputPins(const TWeakObjectPtr<UEdGraphNode> InNode)
{
	TArray<UEdGraphPin*> OutputPins;

	if (InNode.IsValid())
	{
		for (UEdGraphPin* Pin : InNode->GetAllPins())
		{
			if (Pin && Pin->Direction == EGPD_Output && !Pin->GetDisplayName().IsEmpty())
			{
				OutputPins.Add(Pin);
			}
		}
	}
	
	return OutputPins;
}

TArray<UEdGraphPin*> FArrayNodeCreator::GetOutputPins(const TWeakObjectPtr<UEdGraphNode> InNode, const FEdGraphPinType& InSpecificPinType)
{
	const TArray<UEdGraphPin*> OutputPins = GetOutputPins(InNode);
	
	const auto IsSameTypePin = [InSpecificPinType](const UEdGraphPin* InPin)
	{
		return InPin && InPin->PinType == InSpecificPinType;
	};
	
	return OutputPins.FilterByPredicate(IsSameTypePin);
}

TArray<UEdGraphPin*> FArrayNodeCreator::GatherOutputPins(const TArray<TWeakObjectPtr<UEdGraphNode>>& InNodes)
{
	TArray<UEdGraphPin*> OutputPins;

	for (const TWeakObjectPtr<UEdGraphNode>& Node : InNodes)
	{
		const TArray<UEdGraphPin*> NodeOutputPins = GetOutputPins(Node);
		OutputPins.Append(NodeOutputPins);
	}

	return OutputPins;
}

TArray<UEdGraphPin*> FArrayNodeCreator::GatherOutputPins(const TArray<TWeakObjectPtr<UEdGraphNode>>& InNodes,const FEdGraphPinType& InSpecificPinType)
{
	const TArray<UEdGraphPin*> OutputPins = GatherOutputPins(InNodes);
	
	const auto IsSameTypePin = [InSpecificPinType](const UEdGraphPin* InPin)
	{
		return InPin && InPin->PinType == InSpecificPinType;
	};
	
	return OutputPins.FilterByPredicate(IsSameTypePin);
}

bool FArrayNodeCreator::TryGetNodesOutputPinsSameType(const TArray<TWeakObjectPtr<UEdGraphNode>>& InNodes, FEdGraphPinType& OutNodesOutputPinsSameType)
{
	const TArray<UEdGraphPin*> OutputPins = GatherOutputPins(InNodes);
	
	for (const UEdGraphPin* OutputPin : OutputPins)
	{
		if (OutputPin)
		{
			const FEdGraphPinType& OutputPinType = OutputPin->PinType;
			
			const auto IsNodeHasSameTypeOutputPins = [OutputPinType](const TWeakObjectPtr<UEdGraphNode>& InNode)
			{
				return InNode.IsValid() && !GetOutputPins(InNode, OutputPinType).IsEmpty();
			};
		
			const TArray<TWeakObjectPtr<UEdGraphNode>> NodesHasSameTypeOutputPins = InNodes.FilterByPredicate(IsNodeHasSameTypeOutputPins);
		
			if (NodesHasSameTypeOutputPins.Num() == InNodes.Num())
			{
				OutNodesOutputPinsSameType = OutputPinType;
				return true;
			}
		}
	}

	return false;
}

bool FArrayNodeCreator::IsExecutionPinType(const FEdGraphPinType& InPinType)
{
	return InPinType.PinCategory == UEdGraphSchema_K2::PC_Exec;
}

void FArrayNodeCreator::SortPinsByPositionY(TArray<UEdGraphPin*>& OutPins)
{
	TArray<int32> OutPinsIndexes;
	OutPinsIndexes.SetNum(OutPins.Num());

	for (int32 i = 0; i < OutPinsIndexes.Num(); i++)
	{
		OutPinsIndexes[i] = i;
	}

	OutPinsIndexes.StableSort([&OutPins](const int32 InFirstIndex, const int32 InSecondIndex)
	{
		const int32 FirstPinPositionY = GetPinPosition(OutPins[InFirstIndex]).Y;
		const int32 SecondPinPositionY = GetPinPosition(OutPins[InSecondIndex]).Y;
		
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

FIntPoint FArrayNodeCreator::GetPinPosition(const UEdGraphPin* InPin)
{
	if (InPin)
	{
		const UEdGraphNode* PinNode = InPin->GetOwningNode();

		if (IsValid(PinNode))
		{
			return FIntPoint(PinNode->NodePosX, PinNode->NodePosY);
		}
	}
	
	return FIntPoint();
}

FIntPoint FArrayNodeCreator::GetDesiredArrayNodePosition(const TArray<TWeakObjectPtr<UEdGraphNode>>& InNodes)
{
	const int32 DesiredArrayNodePositionX = GetNodesMaxPositionX(InNodes) + 200;
	const int32 DesiredArrayNodePositionY = GetNodesAveragePositionY(InNodes);

	return FIntPoint(DesiredArrayNodePositionX, DesiredArrayNodePositionY);
}

int32 FArrayNodeCreator::GetNodesMaxPositionX(const TArray<TWeakObjectPtr<UEdGraphNode>>& InNodes)
{
	int32 MaxPositionX = 0;

	for (const TWeakObjectPtr<UEdGraphNode>& Node : InNodes)
	{
		if (Node.IsValid())
		{
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
	}

	return MaxPositionX;
}

int32 FArrayNodeCreator::GetNodesAveragePositionY(const TArray<TWeakObjectPtr<UEdGraphNode>>& InNodes)
{
	int32 TotalPositionY = 0;
	
	for (const TWeakObjectPtr<UEdGraphNode>& Node : InNodes)
	{
		if (Node.IsValid())
		{
			TotalPositionY += Node->NodePosY;
		}
	}

	return TotalPositionY / InNodes.Num();
}

#undef LOCTEXT_NAMESPACE
