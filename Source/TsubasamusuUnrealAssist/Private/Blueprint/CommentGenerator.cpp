// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "Blueprint/CommentGenerator.h"
#include "EdGraphNode_Comment.h"
#include "TsubasamusuStringUtility.h"
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
		
		auto AnimationTextChangedFunction = [InCommentNode](const FString& InAnimationText)
		{
			if (InCommentNode.IsValid())
			{
				InCommentNode->OnUpdateCommentText(InAnimationText);
			}
		};
		
		const TSharedRef<FTSTicker::FDelegateHandle> TickerHandle = FTsubasamusuStringUtility::PlayTextAnimation(TEXT("Generating"), AnimationTextChangedFunction);

		const FString Prompt = GetDesiredPrompt(NodeDataListString);
		TSharedPtr<FString> GeneratedComment = MakeShared<FString>();
		
		auto TokenGeneratedFunction = [InCommentNode, GeneratedComment, TickerHandle](const bool bInSucceeded, const FString& InGeneratedToken)
		{
			if (InCommentNode.IsValid())
			{
				if (TickerHandle->IsValid())
				{
					FTSTicker::GetCoreTicker().RemoveTicker(*TickerHandle);
					TickerHandle->Reset();
				}
				
				const FString ErrorMessage = TEXT("Failed to generate comment. For more details, see Output Log.");
				FString DesiredComment = bInSucceeded ? (*GeneratedComment + InGeneratedToken) : ErrorMessage;
				
				InCommentNode->OnUpdateCommentText(*DesiredComment);
				
				if (bInSucceeded)
				{
					*GeneratedComment = DesiredComment;
				}
			}
		};
		
		const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
		ULlmManager::GetChecked()->GenerateToken(Prompt, TokenGeneratedFunction, TsubasamusuUnrealAssistSettings->bEnableStreamingCommentGeneration);
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
    
	FString Prompt = TEXT("Role: Professional Unreal Engine Developer\n");
	Prompt += TEXT("Task: Generate a concise summary comment for a Blueprint logic snippet.\n\n");

	Prompt += TEXT("Constraints:\n");
    
	const FString CommentLanguage = TsubasamusuUnrealAssistSettings->GetCommentGenerationLanguageCulture()->GetEnglishName();
	Prompt += FString::Printf(TEXT("- Language: Answer ONLY in %s.\n"), *CommentLanguage);
    
	Prompt += TEXT("- Format: Output ONLY the comment string itself. Do not include quotes, preamble, or any explanation.\n");

	for (const FString& Condition : TsubasamusuUnrealAssistSettings->CommentGenerationConditions)
	{
		Prompt += FString::Printf(TEXT("- %s\n"), *Condition);
	}

	Prompt += TEXT("\n### Blueprint Node Data:\n");
	Prompt += InNodeDataListString;

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
