// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "Blueprint/CommentGenerator.h"
#include "EdGraphNode_Comment.h"
#include "Algo/AnyOf.h"
#include "Blueprint/NodeInformationUtility.h"
#include "Debug/TsubasamusuLogUtility.h"
#include "Internationalization/Culture.h"
#include "Setting/EditorSettingsUtility.h"
#include "Setting/TsubasamusuUnrealAssistSettings.h"
#include "Subsystem/LlmManager.h"

#define LOCTEXT_NAMESPACE "FCommentGenerator"

void FCommentGenerator::AddCommentGenerationMenu(FMenuBuilder& InMenuBuilder, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
    const TAttribute<FText> LabelText = LOCTEXT("CommentGenerationLabel", "Generate Comment");
    const TAttribute<FText> ToolTipText = LOCTEXT("CommentGenerationToolTip", "Generate a comment based on the nodes contained in the comment node.");

    InMenuBuilder.AddMenuEntry(LabelText, ToolTipText, FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([InCommentNode]()
    {
    	if (InCommentNode.IsValid())
    	{
    		GenerateComment(InCommentNode);
    	}
    })));
}

void FCommentGenerator::GenerateComment(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
	if (InCommentNode.IsValid())
	{
		FString NodeDataListString;

		if (!TryGetNodeDataListStringUnderComment(NodeDataListString, InCommentNode))
		{
			const FString ErrorMessage = TEXT("Failed to get node data list.");
		
			TUA_ERROR(TEXT("%s"), *ErrorMessage);
			InCommentNode->OnUpdateCommentText(ErrorMessage);
		
			return;
		}

		InCommentNode->OnUpdateCommentText(TEXT("Generating comment..."));
		
		const FString Prompt = GetDesiredPrompt(NodeDataListString);
		TSharedPtr<FString> GeneratedComment = MakeShared<FString>();
		
		ULlmManager::GetChecked()->GenerateToken(Prompt, [InCommentNode, GeneratedComment](const bool bInSucceeded, const FString& InGeneratedToken)
		{
			if (InCommentNode.IsValid())
			{
				const FString ErrorMessage = TEXT("Failed to generate comment. For more details, see Output Log.");
				FString DesiredComment = bInSucceeded ? (*GeneratedComment + InGeneratedToken) : ErrorMessage;
				
				InCommentNode->OnUpdateCommentText(*DesiredComment);
				
				if (bInSucceeded)
				{
					*GeneratedComment = DesiredComment;
				}
			}
		});
	}
}

TArray<UEdGraphNode*> FCommentGenerator::GetActiveNodes(const TArray<UEdGraphNode*>& InNodes)
{
	const UTsubasamusuUnrealAssistSettings* Settings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();

	auto IsActiveNode = [Settings](const UEdGraphNode* InNode)
	{
		if (HasConnectedPins(InNode))
		{
			return true;
		}

		if (FNodeInformationUtility::IsCommentNode(InNode))
		{
			return !Settings->bIgnoreCommentNodesWhenGeneratingComments;
		}

		return !Settings->bIgnoreIsolatedNodesWhenGeneratingComments;
	};
	
	return InNodes.FilterByPredicate(IsActiveNode);
}

bool FCommentGenerator::HasConnectedPins(const UEdGraphNode* InNode)
{
	auto IsConnectedPin = [](const UEdGraphPin* InPin)
	{
		return InPin && InPin->HasAnyConnections();
	};
	
	return Algo::AnyOf(InNode->GetAllPins(), IsConnectedPin);
}

FString FCommentGenerator::GetDesiredPrompt(const FString& InNodeDataListString)
{
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
	
	FString Prompt = TEXT("You are developing a game using the Unreal Engine and are going to write a comment in a comment node for a blueprint process represented by the following string in JSON format. Answer the appropriate comment to be written in the comment node according to the following conditions.");

	Prompt += TEXT("\n\n- answer in ") + TsubasamusuUnrealAssistSettings->GetCommentGenerationLanguageCulture()->GetEnglishName();

	for (FString CommentGenerationCondition : TsubasamusuUnrealAssistSettings->CommentGenerationConditions)
	{
		Prompt += TEXT("\n- ") + CommentGenerationCondition;
	}

	Prompt += TEXT("\n\n") + InNodeDataListString;

	return Prompt;
}

TArray<UEdGraphNode*> FCommentGenerator::GetNodesUnderComment(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
	TArray<UEdGraphNode*> NodesUnderComment;

	if (InCommentNode.IsValid())
	{
		TArray<UObject*> NodeObjectsUnderComment = InCommentNode->GetNodesUnderComment();

		for (UObject* NodeObjectUnderComment : NodeObjectsUnderComment)
		{
			UEdGraphNode* Node = Cast<UEdGraphNode>(NodeObjectUnderComment);
		
			if (IsValid(Node))
			{
				NodesUnderComment.Add(Node);
			}
		}
	}

	return NodesUnderComment;
}

bool FCommentGenerator::TryGetNodeDataListStringUnderComment(FString& OutNodeDataListString, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
	if (InCommentNode.IsValid())
	{
		const TArray<UEdGraphNode*> NodesUnderComment = GetNodesUnderComment(InCommentNode);
		const TArray<UEdGraphNode*> ActiveNodesUnderComment = GetActiveNodes(NodesUnderComment);

		if (!ActiveNodesUnderComment.IsEmpty())
		{
			const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
	
			if (TsubasamusuUnrealAssistSettings->bUseToonFormatForCommentGeneration)
			{
				return FNodeInformationUtility::TryGetNodeDataListToonString(OutNodeDataListString, ActiveNodesUnderComment);
			}
	
			return FNodeInformationUtility::TryGetNodeDataListString(OutNodeDataListString, ActiveNodesUnderComment);
		}
	}
	
	return false;
}

#undef LOCTEXT_NAMESPACE
