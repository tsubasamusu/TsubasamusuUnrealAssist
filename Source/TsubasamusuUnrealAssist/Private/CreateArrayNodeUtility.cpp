// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "CreateArrayNodeUtility.h"
#include "GraphNodeUtility.h"
#include "K2Node_MakeArray.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "FCreateArrayNodeUtility"

void FCreateArrayNodeUtility::AddCreateArrayNodeMenu(const UEdGraph* InGraph, FMenuBuilder& MenuBuilder, const TSharedPtr<FEdGraphPinType> ArrayNodePinType)
{
	MenuBuilder.BeginSection(TEXT("TsubasamusuUnrealAssistSection"), LOCTEXT("TsubasamusuUnrealAssistSectionHeader", "Tsubasamusu Unreal Assist"));
	
	const FText CreateArrayNodeLabelText = LOCTEXT("CreateArrayNodeLabelText", "Make Array");
	const FText CreateArrayNodeToolTipText = LOCTEXT("CreateArrayNodeToolTipText", "Make an array with all selected nodes connected.");

	const FSlateIcon MenuIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.MakeArray_16x");

	MenuBuilder.AddMenuEntry(CreateArrayNodeLabelText, CreateArrayNodeToolTipText, MenuIcon, FUIAction(FExecuteAction::CreateLambda([ArrayNodePinType, InGraph]()
	{
		CreateArrayNode(const_cast<UEdGraph*>(InGraph), ArrayNodePinType);
	})));
	
	MenuBuilder.EndSection();
}

void FCreateArrayNodeUtility::CreateArrayNode(UEdGraph* InGraph, const TSharedPtr<FEdGraphPinType> ArrayNodePinType)
{
	if (!IsValid(InGraph))
	{
		return;
	}
	
	const FText TransactionSessionName = LOCTEXT("CreateArrayNodeTransaction", "Create Array Node");
	FScopedTransaction Transaction(TransactionSessionName);

	InGraph->Modify();
	
	UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(InGraph);
	
	if (IsValid(Blueprint))
	{
		Blueprint->Modify();
	}
	
	const TArray<UEdGraphNode*> SelectedNodes = FGraphNodeUtility::GetSelectedNodes(InGraph);
	TArray<UEdGraphPin*> SelectedNodesOutputPins = FGraphNodeUtility::GetNodesOutputPins(SelectedNodes, *ArrayNodePinType);
	FGraphNodeUtility::SortPinsByPositionY(SelectedNodesOutputPins);
	
	FGraphNodeCreator<UK2Node_MakeArray> ArrayNodeCreator(*InGraph);
	
	UK2Node_MakeArray* CreatedArrayNode = ArrayNodeCreator.CreateNode();
	
	if (!IsValid(CreatedArrayNode))
	{
		return;
	}
	
	CreatedArrayNode->AllocateDefaultPins();

	const FIntPoint DesiredArrayNodePosition = GetDesiredArrayNodePosition(InGraph);
	CreatedArrayNode->NodePosX = DesiredArrayNodePosition.X;
	CreatedArrayNode->NodePosY = DesiredArrayNodePosition.Y;

	for (int32 i = 0; i < SelectedNodesOutputPins.Num() - 1; i++)
	{
		CreatedArrayNode->AddInputPin();
	}

	ArrayNodeCreator.Finalize();
	
	const TArray<UEdGraphPin*> CreatedArrayNodeInputPins = FGraphNodeUtility::GetNodeInputPins(CreatedArrayNode);

	for (UEdGraphPin* SelectedNodesOutputPin : SelectedNodesOutputPins)
	{
		if (!SelectedNodesOutputPin)
		{
			continue;
		}

		for (UEdGraphPin* CreatedArrayNodeInputPin : CreatedArrayNodeInputPins)
		{
			if (!CreatedArrayNodeInputPin || CreatedArrayNodeInputPin->HasAnyConnections())
			{
				continue;
			}

			if (InGraph->GetSchema()->TryCreateConnection(CreatedArrayNodeInputPin, SelectedNodesOutputPin))
			{
				break;
			}
		}
	}
	
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
}

FIntPoint FCreateArrayNodeUtility::GetDesiredArrayNodePosition(const UEdGraph* InGraph)
{
	const TArray<UEdGraphNode*> SelectedNodes = FGraphNodeUtility::GetSelectedNodes(InGraph);

	const int32 DesiredArrayNodePositionX = GetNodesMaxPositionX(SelectedNodes) + 200;
	const int32 DesiredArrayNodePositionY = GetNodesAveragePositionY(SelectedNodes);

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
