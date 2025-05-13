// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "CreateArrayNodeUtility.h"
#include "GraphEditorModule.h"
#include "K2Node_CallFunction.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "FCreateArrayNodeUtility"

void FCreateArrayNodeUtility::RegisterSelectedNodeMenu()
{
	FGraphEditorModule& GraphEditorModule = FModuleManager::LoadModuleChecked<FGraphEditorModule>("GraphEditor");

	GraphEditorModule.GetAllGraphEditorContextMenuExtender().Add(FGraphEditorModule::FGraphEditorMenuExtender_SelectedNode::CreateStatic(ExtendSelectedNodeMenu));
}

TSharedRef<FExtender> FCreateArrayNodeUtility::ExtendSelectedNodeMenu(TSharedRef<FUICommandList> CommandList, const UEdGraph* InGraph, const UEdGraphNode* InNode, const UEdGraphPin* InPin, bool bIsDebugging)
{
	TSharedRef<FExtender> Extender = MakeShared<FExtender>();

	TSharedPtr<FEdGraphPinType> SelectedNodesOutputPinsCommonType = MakeShared<FEdGraphPinType>();

	const TArray<UEdGraphNode*> SelectedNodes = GetSelectedNodes(InGraph);
	
	if (TryGetNodesOutputPinsCommonType(SelectedNodes, *SelectedNodesOutputPinsCommonType))
	{
		Extender->AddMenuExtension("SchemaActionComment", EExtensionHook::After, nullptr, FMenuExtensionDelegate::CreateLambda([InGraph, SelectedNodesOutputPinsCommonType](FMenuBuilder& MenuBuilder)
		{
			AddCreateArrayNodeMenu(InGraph, MenuBuilder, SelectedNodesOutputPinsCommonType);
		}));
	}
	
	return Extender;
}

void FCreateArrayNodeUtility::AddCreateArrayNodeMenu(const UEdGraph* InGraph, FMenuBuilder& MenuBuilder, const TSharedPtr<FEdGraphPinType> ArrayNodePinType)
{
	const FText CreateArrayNodeLabelText = LOCTEXT("CreateArrayNodeLabelText", "Create Array Node");
	const FText CreateArrayNodeToolTipText = LOCTEXT("CreateArrayNodeToolTipText", "Create an array node with all selected nodes connected.");

	const FSlateIcon MenuIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.MakeArray_16x");
	
	MenuBuilder.AddMenuEntry(CreateArrayNodeLabelText, CreateArrayNodeToolTipText, MenuIcon, FUIAction(FExecuteAction::CreateLambda([ArrayNodePinType, InGraph]()
	{
		CreateArrayNode(InGraph, ArrayNodePinType);
	})));
}

void FCreateArrayNodeUtility::CreateArrayNode(const UEdGraph* InGraph, const TSharedPtr<FEdGraphPinType> ArrayNodePinType)
{
}

FIntPoint FCreateArrayNodeUtility::GetDesiredArrayNodePosition(const UEdGraph* InGraph)
{
	const TArray<UEdGraphNode*> SelectedNodes = GetSelectedNodes(InGraph);

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

TArray<UEdGraphNode*> FCreateArrayNodeUtility::GetSelectedNodes(const UEdGraph* InGraph)
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

TArray<UEdGraphPin*> FCreateArrayNodeUtility::GetNodesOutputPins(const TArray<UEdGraphNode*>& InNodes)
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

bool FCreateArrayNodeUtility::TryGetNodesOutputPinsCommonType(const TArray<UEdGraphNode*>& InNodes, FEdGraphPinType& OutNodesOutputPinsCommonType)
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

bool FCreateArrayNodeUtility::NodeHasSameTypeOutputPins(const UEdGraphNode* InNode, const FEdGraphPinType& InPinType)
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

#undef LOCTEXT_NAMESPACE
