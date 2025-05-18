// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "SelectedNodeMenuExtender.h"
#include "CreateArrayNodeUtility.h"
#include "GraphEditorModule.h"
#include "GraphNodeUtility.h"

void FSelectedNodeMenuExtender::RegisterSelectedNodeMenu()
{
	FGraphEditorModule& GraphEditorModule = FModuleManager::LoadModuleChecked<FGraphEditorModule>("GraphEditor");

	GraphEditorModule.GetAllGraphEditorContextMenuExtender().Add(FGraphEditorModule::FGraphEditorMenuExtender_SelectedNode::CreateStatic(ExtendSelectedNodeMenu));
}

TSharedRef<FExtender> FSelectedNodeMenuExtender::ExtendSelectedNodeMenu(TSharedRef<FUICommandList> CommandList, const UEdGraph* InGraph, const UEdGraphNode* InNode, const UEdGraphPin* InPin, bool bIsDebugging)
{
	TSharedRef<FExtender> Extender = MakeShared<FExtender>();

	TSharedPtr<FEdGraphPinType> SelectedNodesOutputPinsCommonType = MakeShared<FEdGraphPinType>();

	const TArray<UEdGraphNode*> SelectedNodes = FGraphNodeUtility::GetSelectedNodes(InGraph);
	
	if (FGraphNodeUtility::TryGetNodesOutputPinsCommonType(SelectedNodes, *SelectedNodesOutputPinsCommonType))
	{
		Extender->AddMenuExtension("SchemaActionComment", EExtensionHook::After, nullptr, FMenuExtensionDelegate::CreateLambda([InGraph, SelectedNodesOutputPinsCommonType](FMenuBuilder& MenuBuilder)
		{
			FCreateArrayNodeUtility::AddCreateArrayNodeMenu(InGraph, MenuBuilder, SelectedNodesOutputPinsCommonType);
		}));
	}
	
	return Extender;
}
