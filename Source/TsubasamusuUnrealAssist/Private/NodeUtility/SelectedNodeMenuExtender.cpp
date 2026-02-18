// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "NodeUtility/SelectedNodeMenuExtender.h"
#include "NodeUtility/CommentTranslator.h"
#include "NodeUtility/ArrayNodeCreator.h"
#include "EdGraphNode_Comment.h"
#include "GraphEditorModule.h"
#include "NodeUtility/NodeUtility.h"
#include "NodeUtility/NodeInformationCopier.h"
#include "NodeUtility/CommentGenerator.h"

void FSelectedNodeMenuExtender::RegisterSelectedNodeMenu()
{
	FGraphEditorModule& GraphEditorModule = FModuleManager::LoadModuleChecked<FGraphEditorModule>(TEXT("GraphEditor"));
	GraphEditorModule.GetAllGraphEditorContextMenuExtender().Add(FGraphEditorModule::FGraphEditorMenuExtender_SelectedNode::CreateStatic(ExtendSelectedNodeMenu));
}

TSharedRef<FExtender> FSelectedNodeMenuExtender::ExtendSelectedNodeMenu(TSharedRef<FUICommandList> CommandList, const UEdGraph* InGraph, const UEdGraphNode* InNode, const UEdGraphPin* InPin, bool bIsDebugging)
{
	TSharedRef<FExtender> Extender = MakeShared<FExtender>();
	const TArray<UEdGraphNode*> SelectedNodes = FNodeUtility::GetSelectedNodes(InGraph);

	if (SelectedNodes.Num() == 0)
	{
		return Extender;
	}
	
	const TWeakObjectPtr<UEdGraph> WeakGraph = const_cast<UEdGraph*>(InGraph);

	// Multiple Nodes Menu
	if (SelectedNodes.Num() >= 2)
	{
		// Copy Node Information
		Extender->AddMenuExtension(TEXT("SchemaActionComment"), EExtensionHook::After, nullptr, FMenuExtensionDelegate::CreateLambda([WeakGraph](FMenuBuilder& MenuBuilder)
		{
			FNodeInformationCopier::AddCopyNodeInformationMenu(WeakGraph, MenuBuilder);
		}));

		// Create Array Node
		{
			TSharedPtr<FEdGraphPinType> SelectedNodesOutputPinsCommonType = MakeShared<FEdGraphPinType>();

			if (FNodeUtility::TryGetNodesOutputPinsCommonType(SelectedNodes, *SelectedNodesOutputPinsCommonType))
			{
				if (!FNodeUtility::IsExecPinType(*SelectedNodesOutputPinsCommonType.Get()))
				{
					Extender->AddMenuExtension(TEXT("SchemaActionComment"), EExtensionHook::After, nullptr, FMenuExtensionDelegate::CreateLambda([WeakGraph, SelectedNodesOutputPinsCommonType](FMenuBuilder& MenuBuilder)
					{
						FArrayNodeCreator::AddCreateArrayNodeMenu(WeakGraph, MenuBuilder, SelectedNodesOutputPinsCommonType);
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
			
			Extender->AddMenuExtension(TEXT("GraphNodeComment"), EExtensionHook::After, nullptr, FMenuExtensionDelegate::CreateLambda([WeakCommentNode](FMenuBuilder& MenuBuilder)
			{
				FCommentTranslator::AddCommentTranslationMenu(MenuBuilder, WeakCommentNode);
				FCommentGenerator::AddCommentGenerationMenu(MenuBuilder, WeakCommentNode);
			}));
		}
	}

	return Extender;
}
