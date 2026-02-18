// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "Blueprint/CommentGenerator.h"
#include "EdGraphNode_Comment.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Blueprint/NodeInformationUtility.h"
#include "Debug/TsubasamusuLogUtility.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Internationalization/Culture.h"
#include "Setting/EditorSettingsUtility.h"
#include "Setting/TsubasamusuUnrealAssistSettings.h"
#include "Type/TsubasamusuUnrealAssistStructs.h"

#define LOCTEXT_NAMESPACE "FCommentGenerator"

void FCommentGenerator::AddCommentGenerationMenu(FMenuBuilder& InMenuBuilder, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
    const TAttribute<FText> LabelText = LOCTEXT("CommentGenerationLabel", "Generate Comment");
    const TAttribute<FText> ToolTipText = LOCTEXT("CommentGenerationToolTip", "Generate a comment based on the nodes contained in the comment node.");

    InMenuBuilder.AddMenuEntry(LabelText, ToolTipText, FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([InCommentNode]()
    {
    	if (InCommentNode.IsValid())
    	{
    		UpdateCommentByGpt(InCommentNode);
    	}
    })));
}

void FCommentGenerator::UpdateCommentByGpt(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
	if (!InCommentNode.IsValid())
	{
		return;
	}
	
	FString NodeDataListString;

	if (!TryGetNodeDataListStringUnderComment(NodeDataListString, InCommentNode))
	{
		const FString ErrorMessage = TEXT("Failed to get a node data list as JSON string.");
		
		TUA_ERROR(TEXT("%s"), *ErrorMessage);
		InCommentNode->OnUpdateCommentText(ErrorMessage);
		
		return;
	}

	InCommentNode->OnUpdateCommentText(TEXT("Start generating comment..."));

	GenerateComment(NodeDataListString, [InCommentNode](const bool bSucceeded, const FString& Message)
	{
		if (!InCommentNode.IsValid())
		{
			return;
		}
		
		InCommentNode->OnUpdateCommentText(Message);

		if (!bSucceeded)
		{
			TUA_ERROR(TEXT("%s"), *Message);
		}
	});
}

TArray<UEdGraphNode*> FCommentGenerator::GetActiveNodes(const TArray<UEdGraphNode*>& InNodes)
{
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
	
	TArray<UEdGraphNode*> ActiveNodes;

	for (UEdGraphNode* Node : InNodes)
	{
		if (HasConnectedPins(Node))
		{
			ActiveNodes.Add(Node);

			continue;
		}

		if (FNodeInformationUtility::IsCommentNode(Node))
		{
			if (!TsubasamusuUnrealAssistSettings->bIgnoreCommentNodesWhenGeneratingComments)
			{
				ActiveNodes.Add(Node);
			}

			continue;
		}

		if (!TsubasamusuUnrealAssistSettings->bIgnoreIsolatedNodesWhenGeneratingComments)
		{
			ActiveNodes.Add(Node);
		}
	}

	return ActiveNodes;
}

bool FCommentGenerator::HasConnectedPins(const UEdGraphNode* InNode)
{
	TArray<UEdGraphPin*> Pins = InNode->GetAllPins();
	
	for (const UEdGraphPin* Pin : Pins)
	{
		if (Pin->HasAnyConnections())
		{
			return true;
		}
	}

	return false;
}

int32 FCommentGenerator::GetCharNum(const FString& InString, const TCHAR& InChar)
{
	int32 CharNum = 0;

	for (const TCHAR Char : InString)
	{
		if (Char == InChar)
		{
			CharNum++;
		}
	}

	return CharNum;
}

void FCommentGenerator::GenerateComment(const FString& NodeDataListString, const TFunction<void(const bool bSucceeded, const FString& Message)>& OnGeneratedComment)
{
	FString GptRequestString;

	if (!TryGetGptRequestString(NodeDataListString, GptRequestString))
	{
		OnGeneratedComment(false, TEXT("Failed to create a GPT request."));
		return;
	}

	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
	const FString OpenAiApiKey = TsubasamusuUnrealAssistSettings->OpenAiApiKey;

	const TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->SetURL(TEXT("https://api.openai.com/v1/chat/completions"));
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Authorization"), TEXT("Bearer ") + OpenAiApiKey);
	HttpRequest->SetContentAsString(GptRequestString);

	HttpRequest->OnProcessRequestComplete().BindLambda([OnGeneratedComment](FHttpRequestPtr, const FHttpResponsePtr& HttpResponsePtr, const bool bSucceeded)
	{
		if (!bSucceeded)
		{
			OnGeneratedComment(false, TEXT("Failed to send a HTTP request."));
			return;
		}

		if (!HttpResponsePtr.IsValid())
		{
			OnGeneratedComment(false, TEXT("Failed to get a HTTP response."));
			return;
		}

		const FString JsonResponse = HttpResponsePtr->GetContentAsString();

		FGptErrorResponse GptErrorResponse;

		if (FJsonObjectConverter::JsonObjectStringToUStruct(JsonResponse, &GptErrorResponse, 0, 0) && !GptErrorResponse.IsEmpty())
		{
			OnGeneratedComment(false, GptErrorResponse.error.message);
			return;
		}

		FGptResponse GptResponse;

		if (!FJsonObjectConverter::JsonObjectStringToUStruct(JsonResponse, &GptResponse, 0, 0))
		{
			OnGeneratedComment(false, TEXT("Failed to get a GPT response."));
			return;
		}

		if (GptResponse.IsEmpty())
		{
			OnGeneratedComment(false, TEXT("Failed to get a GPT response."));
			return;
		}

		OnGeneratedComment(true, GptResponse.GetGptMessage());
	});

	if (!HttpRequest->ProcessRequest())
	{
		OnGeneratedComment(false, TEXT("Failed to process a HTTP request."));
	}
}

bool FCommentGenerator::TryGetGptRequestString(const FString& NodeDataListString, FString& OutGptRequestString)
{
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();

#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION <= 2)
	FGptMessage GptMessage;
	GptMessage.role = TEXT("user");
	GptMessage.content = GetDesiredPrompt(NodeDataListString);
	
	FGptRequest GptRequest;
	GptRequest.model = TsubasamusuUnrealAssistSettings->GptModelName;
	GptRequest.messages.Add(GptMessage);
#else
	const FGptRequest GptRequest =
	{
		.model = TsubasamusuUnrealAssistSettings->GptModelName,
		.messages =
		{
			{
				.role = TEXT("user"),
				.content = GetDesiredPrompt(NodeDataListString)
			}
		}
	};
#endif

	return FJsonObjectConverter::UStructToJsonObjectString(GptRequest, OutGptRequestString, 0, 0);
}

FString FCommentGenerator::GetDesiredPrompt(const FString& NodeDataListString)
{
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
	
	FString Prompt = TEXT("You are developing a game using the Unreal Engine and are going to write a comment in a comment node for a blueprint process represented by the following string in JSON format. Answer the appropriate comment to be written in the comment node according to the following conditions.");

	Prompt += TEXT("\n\n- answer in ") + TsubasamusuUnrealAssistSettings->GetCommentGenerationLanguageCulture()->GetEnglishName();

	for (FString CommentGenerationCondition : TsubasamusuUnrealAssistSettings->CommentGenerationConditions)
	{
		Prompt += TEXT("\n- ") + CommentGenerationCondition;
	}

	Prompt += TEXT("\n\n") + NodeDataListString;

	return Prompt;
}

TArray<UEdGraphNode*> FCommentGenerator::GetNodesUnderComment(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
	//TODO: コメントノード内のノードの更新
	// HandleSelection(true, true) を SGraphNodeComment::OnMouseButtonUp() 経由で間接的に呼び出すか？

	TArray<UEdGraphNode*> NodesUnderComment;

	if (!InCommentNode.IsValid())
	{
		return NodesUnderComment;
	}

	TArray<UObject*> NodeObjectsUnderComment = InCommentNode->GetNodesUnderComment();

	for (UObject* NodeObjectUnderComment : NodeObjectsUnderComment)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(NodeObjectUnderComment);
		
		if (IsValid(Node))
		{
			NodesUnderComment.Add(Node);
		}
	}

	return NodesUnderComment;
}

bool FCommentGenerator::TryGetNodeDataListStringUnderComment(FString& OutNodeDataListString, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
	if (!InCommentNode.IsValid())
	{
		return false;
	}
	
	const TArray<UEdGraphNode*> NodesUnderComment = GetNodesUnderComment(InCommentNode);
	const TArray<UEdGraphNode*> ActiveNodesUnderComment = GetActiveNodes(NodesUnderComment);

	if (ActiveNodesUnderComment.Num() == 0)
	{
		return false;
	}
	
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
	
	if (TsubasamusuUnrealAssistSettings->bUseToonFormatForCommentGeneration)
	{
		return FNodeInformationUtility::TryGetNodeDataListToonString(OutNodeDataListString, ActiveNodesUnderComment);
	}
	
	return FNodeInformationUtility::TryGetNodeDataListString(OutNodeDataListString, ActiveNodesUnderComment);
}

#undef LOCTEXT_NAMESPACE
