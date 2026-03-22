// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "Blueprint/ArrayNodeCreator.h"
#include "K2Node_MakeArray.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Type/TsubasamusuUnrealAssistStructs.h"
#include "Type/TsubasamusuUnrealAssistMacros.h"

#define LOCTEXT_NAMESPACE "FArrayNodeCreator"

FSelectedNodeMenuContext FArrayNodeCreator::CreateSelectedNodeMenuContext()
{
	const FShouldAddMenu ShouldAddMenu = [](const TArray<TWeakObjectPtr<UEdGraphNode>>& InSelectedNodes)
	{
		const auto IsValidNode = [](const TWeakObjectPtr<UEdGraphNode>& InNode)
		{
			return InNode.IsValid();	
		};
		
		const TArray<TWeakObjectPtr<UEdGraphNode>> ValidSelectedNodes = InSelectedNodes.FilterByPredicate(IsValidNode);
		FEdGraphPinType SelectedNodesOutputPinsSameType;
		
		if (ValidSelectedNodes.Num() >= 2 && TryGetNodesOutputPinsSameType(ValidSelectedNodes, SelectedNodesOutputPinsSameType))
		{
			return !IsExecutionPinType(SelectedNodesOutputPinsSameType);
		}
		
		return false;
	};
	
	const FOnSelectedNodeMenuClicked OnClicked = [](const TArray<TWeakObjectPtr<UEdGraphNode>>& InSelectedNodes, const TWeakObjectPtr<UEdGraph> InGraph)
	{
		FEdGraphPinType SelectedNodesOutputPinsSameType;
		
		if (TryGetNodesOutputPinsSameType(InSelectedNodes, SelectedNodesOutputPinsSameType))
		{
			CreateArrayNode(InSelectedNodes, InGraph, SelectedNodesOutputPinsSameType);
		}
	};
	
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 3, 0)
	return FSelectedNodeMenuContext
	{
		.ShouldAddMenu = ShouldAddMenu,
		.OnClicked = OnClicked,
		.LabelText = LOCTEXT("CreateArrayNodeLabel", "Make Array"),
		.ToolTipText = LOCTEXT("CreateArrayNodeToolTip", "Make an array with all selected nodes connected."),
		.MenuIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.MakeArray_16x")
	};
#else
	FSelectedNodeMenuContext SelectedNodeMenuContext;
	
	SelectedNodeMenuContext.ShouldAddMenu = ShouldAddMenu;
	SelectedNodeMenuContext.OnClicked = OnClicked;
	SelectedNodeMenuContext.LabelText = LOCTEXT("CreateArrayNodeLabel", "Make Array");
	SelectedNodeMenuContext.ToolTipText = LOCTEXT("CreateArrayNodeToolTip", "Make an array with all selected nodes connected.");
	SelectedNodeMenuContext.MenuIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.MakeArray_16x");
	
	return SelectedNodeMenuContext;
#endif
}

UK2Node_MakeArray* FArrayNodeCreator::CreateArrayNode(const TArray<TWeakObjectPtr<UEdGraphNode>>& InNodes, const TWeakObjectPtr<UEdGraph> InGraph, const FEdGraphPinType& InArrayNodePinType)
{
	UK2Node_MakeArray* ArrayNode = nullptr;
	
	if (!InNodes.IsEmpty() && InGraph.IsValid())
	{
		UEdGraph* Graph = InGraph.Get();
		UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(Graph);
	
		if (IsValid(Blueprint))
		{
			TArray<UEdGraphPin*> OutputPins = GatherOutputPins(InNodes, InArrayNodePinType);
			SortPinsByPositionY(OutputPins);
			
			FGraphNodeCreator<UK2Node_MakeArray> ArrayNodeCreator(*Graph);
			ArrayNode = ArrayNodeCreator.CreateNode();
			
			if (IsValid(ArrayNode))
			{
				FScopedTransaction Transaction(LOCTEXT("CreateArrayNodeTransaction", "Create Array Node"));
		
				Graph->Modify();
				Blueprint->Modify();
	
				ArrayNode->AllocateDefaultPins();
			
				const FIntPoint DesiredArrayNodePosition = GetDesiredArrayNodePosition(InNodes);
				ArrayNode->NodePosX = DesiredArrayNodePosition.X;
				ArrayNode->NodePosY = DesiredArrayNodePosition.Y;
				
				for (int32 i = 0; i < OutputPins.Num() - 1; i++)
				{
					ArrayNode->AddInputPin();
				}
				
				ArrayNodeCreator.Finalize();
				
				const TArray<UEdGraphPin*> ArrayNodeInputPins = GetInputPins(ArrayNode);
				
				for (UEdGraphPin* OutputPin : OutputPins)
				{
					if (OutputPin)
					{
						for (UEdGraphPin* ArrayNodeInputPin : ArrayNodeInputPins)
						{
							if (ArrayNodeInputPin && !ArrayNodeInputPin->HasAnyConnections())
							{
								if (Graph->GetSchema()->TryCreateConnection(ArrayNodeInputPin, OutputPin))
								{
									break;
								}
							}
						}
					}
				}
			
				FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
			}
		}
	}
	
	return ArrayNode;
}

TArray<UEdGraphPin*> FArrayNodeCreator::GetInputPins(const UEdGraphNode* InNode)
{
	TArray<UEdGraphPin*> InputPins;

	if (IsValid(InNode))
	{
		for (UEdGraphPin* Pin : InNode->GetAllPins())
		{
			if (Pin->Direction == EGPD_Input)
			{
				InputPins.Add(Pin);
			}
		}
	}

	return InputPins;
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
