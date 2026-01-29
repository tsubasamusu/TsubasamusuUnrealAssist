// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "BlueprintEditor/Menu/SelectedNodeMenuExtender.h"
#include "BlueprintEditor/Menu/CommentTranslationUtility.h"
#include "BlueprintEditor/Menu/CreateArrayNodeUtility.h"
#include "EdGraphNode_Comment.h"
#include "GraphEditorModule.h"
#include  "BlueprintEditor/GraphNodeUtility.h"
#include "BlueprintEditor/Menu/CopyNodeInformationUtility.h"
#include "BlueprintEditor/Menu/CommentGeneration/CommentGenerationUtility.h"

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

	// Multiple Nodes Menu
	if (SelectedNodes.Num() >= 2)
	{
		// Copy Node Information
		Extender->AddMenuExtension("SchemaActionComment", EExtensionHook::After, nullptr, FMenuExtensionDelegate::CreateLambda([WeakGraph](FMenuBuilder& MenuBuilder)
		{
			FCopyNodeInformationUtility::AddCopyNodeInformationMenu(WeakGraph, MenuBuilder);
		}));

		// Create Array Node
		{
			TSharedPtr<FEdGraphPinType> SelectedNodesOutputPinsCommonType = MakeShared<FEdGraphPinType>();

			if (FGraphNodeUtility::TryGetNodesOutputPinsCommonType(SelectedNodes, *SelectedNodesOutputPinsCommonType))
			{
				if (!FGraphNodeUtility::IsExecPinType(*SelectedNodesOutputPinsCommonType.Get()))
				{
					Extender->AddMenuExtension("SchemaActionComment", EExtensionHook::After, nullptr, FMenuExtensionDelegate::CreateLambda([WeakGraph, SelectedNodesOutputPinsCommonType](FMenuBuilder& MenuBuilder)
					{
						FCreateArrayNodeUtility::AddCreateArrayNodeMenu(WeakGraph, MenuBuilder, SelectedNodesOutputPinsCommonType);
					}));
				}
			}
		}
	}
	// Single Node Menu
	else
	{
		UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(SelectedNodes[0]);

		// Comment Node Menu
		if (IsValid(CommentNode))
		{
			const TWeakObjectPtr<UEdGraphNode_Comment> WeakCommentNode = CommentNode;
			
			Extender->AddMenuExtension("GraphNodeComment", EExtensionHook::After, nullptr, FMenuExtensionDelegate::CreateLambda([WeakCommentNode](FMenuBuilder& MenuBuilder)
			{
				FCommentTranslationUtility::AddCommentTranslationMenu(MenuBuilder, WeakCommentNode);
				FCommentGenerationUtility::AddCommentGenerationMenu(MenuBuilder, WeakCommentNode);
			}));
		}
	}

	return Extender;
}
