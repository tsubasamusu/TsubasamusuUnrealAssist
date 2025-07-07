// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "BlueprintEditor/Menu/SelectedNodeMenuExtender.h"
#include "BlueprintEditor/Menu/CommentTranslationUtility.h"
#include "BlueprintEditor/Menu/CreateArrayNodeUtility.h"
#include "EdGraphNode_Comment.h"
#include "GraphEditorModule.h"
#include  "BlueprintEditor/GraphNodeUtility.h"

void FSelectedNodeMenuExtender::RegisterSelectedNodeMenu()
{
	FGraphEditorModule& GraphEditorModule = FModuleManager::LoadModuleChecked<FGraphEditorModule>("GraphEditor");
	GraphEditorModule.GetAllGraphEditorContextMenuExtender().Add(FGraphEditorModule::FGraphEditorMenuExtender_SelectedNode::CreateStatic(ExtendSelectedNodeMenu));
}

TSharedRef<FExtender> FSelectedNodeMenuExtender::ExtendSelectedNodeMenu(TSharedRef<FUICommandList> CommandList, const UEdGraph* InGraph, const UEdGraphNode* InNode, const UEdGraphPin* InPin, bool bIsDebugging)
{
	TSharedRef<FExtender> Extender = MakeShared<FExtender>();
	const TArray<UEdGraphNode*> SelectedNodes = FGraphNodeUtility::GetSelectedNodes(InGraph);

	if (SelectedNodes.Num() == 0)
	{
		return Extender;
	}
	
	const TWeakObjectPtr<UEdGraph> WeakGraph = const_cast<UEdGraph*>(InGraph);
	
	// Create Array Node
	if (SelectedNodes.Num() >= 2)
	{
		TSharedPtr<FEdGraphPinType> SelectedNodesOutputPinsCommonType = MakeShared<FEdGraphPinType>();

		if (FGraphNodeUtility::TryGetNodesOutputPinsCommonType(SelectedNodes, *SelectedNodesOutputPinsCommonType))
		{
			Extender->AddMenuExtension("SchemaActionComment", EExtensionHook::After, nullptr, FMenuExtensionDelegate::CreateLambda([WeakGraph, SelectedNodesOutputPinsCommonType](FMenuBuilder& MenuBuilder)
			{
				FCreateArrayNodeUtility::AddCreateArrayNodeMenu(WeakGraph, MenuBuilder, SelectedNodesOutputPinsCommonType);
			}));
		}

		return Extender;
	}

	// Translate Comment Node
	{
		UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(SelectedNodes[0]);

		if (IsValid(CommentNode))
		{
			const TWeakObjectPtr<UEdGraphNode_Comment> WeakCommentNode = CommentNode;
			
			Extender->AddMenuExtension("GraphNodeComment", EExtensionHook::After, nullptr, FMenuExtensionDelegate::CreateLambda([WeakCommentNode](FMenuBuilder& MenuBuilder)
			{
				FCommentTranslationUtility::AddCommentTranslationMenu(MenuBuilder, WeakCommentNode);
			}));
		}
	}

	return Extender;
}
