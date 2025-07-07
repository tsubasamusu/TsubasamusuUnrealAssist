// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "BlueprintEditor/Menu/CommentGeneration/CommentGenerationUtility.h"
#include "EdGraphNode_Comment.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "BlueprintEditor/Menu/CommentGeneration/GptResponse.h"
#include "BlueprintEditor/Menu/CommentGeneration/NodeData.h"
#include "Debug/TsubasamusuLogUtility.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Internationalization/Culture.h"
#include "Setting/TsubasamusuUnrealAssistSettings.h"

#define LOCTEXT_NAMESPACE "CommentGenerationUtility"

void FCommentGenerationUtility::AddCommentGenerationMenu(FMenuBuilder& InMenuBuilder, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
    const TAttribute LabelText = LOCTEXT("CommentGenerationLabelText", "Generate Comment");
    const TAttribute ToolTipText = LOCTEXT("CommentGenerationToolTipText", "Generate a comment based on the nodes contained in the comment node.");

    InMenuBuilder.AddMenuEntry(LabelText, ToolTipText, FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([InCommentNode]()
    {
    	if (InCommentNode.IsValid())
    	{
    		UpdateCommentByGpt(InCommentNode);
    	}
    })));
	
    InMenuBuilder.EndSection();
}

void FCommentGenerationUtility::UpdateCommentByGpt(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
	if (!InCommentNode.IsValid())
	{
		return;
	}
	
	FString NodeDataListString;

	if (!TryGetNodeDataListStringUnderComment(NodeDataListString, InCommentNode))
	{
		const FString ErrorMessage = TEXT("Failed to get a node data list as JSON string.");
		
		FTsubasamusuLogUtility::LogError(ErrorMessage);
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
			FTsubasamusuLogUtility::LogError(Message);
		}
	});
}

FNodeDataList FCommentGenerationUtility::GetNodeDataList(const TArray<UEdGraphNode*>& InNodes)
{
	TArray<FNodeData> NodesDataList;

	for (const UEdGraphNode* Node : InNodes)
	{
		FNodeData NodeData =
		{
			.NodeName = Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString(),
			.Comment = Node->NodeComment,
			.bIsCommentNode = IsCommentNode(Node),
			.PinDataList = GetPinDataList(Node)
		};

		NodesDataList.Add(NodeData);
	}

	return FNodeDataList(NodesDataList);
}

TArray<FPinData> FCommentGenerationUtility::GetPinDataList(const UEdGraphNode* InNode)
{
	TArray<FPinData> PinDataList;

	for (const UEdGraphPin* Pin : InNode->GetAllPins())
	{
		FPinData PinData
		{
			.PinName = Pin->GetDisplayName().IsEmpty() ? Pin->PinName.ToString() : Pin->GetDisplayName().ToString(),
			.PinType = GetPinTypeAsString(Pin),
			.PinId = Pin->PinId.ToString(),
			.DefaultValue = Pin->GetDefaultAsString(),
			.bDefaultValueIsUsed = IsPinUsesDefaultValue(Pin),
			.ConnectedPinIds = GetPinIds(Pin->LinkedTo)
		};

		PinDataList.Add(PinData);
	}

	return PinDataList;
}

TArray<FString> FCommentGenerationUtility::GetPinIds(const TArray<UEdGraphPin*>& InPins)
{
	TArray<FString> PinIds;

	for (const UEdGraphPin* Pin : InPins)
	{
		PinIds.Add(Pin->PinId.ToString());
	}

	return PinIds;
}

TArray<UEdGraphNode*> FCommentGenerationUtility::GetActiveNodes(const TArray<UEdGraphNode*>& InNodes)
{
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();

	TArray<UEdGraphNode*> ActiveNodes;

	for (UEdGraphNode* Node : InNodes)
	{
		if (HasConnectedPins(Node))
		{
			ActiveNodes.Add(Node);

			continue;
		}

		if (IsCommentNode(Node))
		{
			if (!TsubasamusuUnrealAssistSettings->bIgnoreCommentNodes)
			{
				ActiveNodes.Add(Node);
			}

			continue;
		}

		if (!TsubasamusuUnrealAssistSettings->bIgnoreNodesDoNotHaveConnectedPins)
		{
			ActiveNodes.Add(Node);
		}
	}

	return ActiveNodes;
}

bool FCommentGenerationUtility::HasConnectedPins(const UEdGraphNode* InNode)
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

bool FCommentGenerationUtility::IsCommentNode(const UEdGraphNode* InNode)
{
	return IsValid(Cast<UEdGraphNode_Comment>(InNode));
}

bool FCommentGenerationUtility::IsPinUsesDefaultValue(const UEdGraphPin* InPin)
{
	return !InPin->HasAnyConnections() && InPin->Direction == EGPD_Input;
}

int32 FCommentGenerationUtility::GetCharNum(const FString& InString, const TCHAR& InChar)
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

FString FCommentGenerationUtility::GetPinTypeAsString(const UEdGraphPin* InPin)
{
	switch (InPin->Direction)
	{
	case EGPD_Input:
		return TEXT("Input");
	case EGPD_Output:
		return TEXT("Output");
	default:
		return TEXT("UnknownType");
	}
}

void FCommentGenerationUtility::GenerateComment(const FString& NodeDataListString, const TFunction<void(const bool bSucceeded, const FString& Message)>& OnGeneratedComment)
{
	FString GptRequestString;

	if (!TryGetGptRequestString(NodeDataListString, GptRequestString))
	{
		OnGeneratedComment(false, TEXT("Failed to create a GPT request."));
		return;
	}

	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();
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

bool FCommentGenerationUtility::TryGetGptRequestString(const FString& NodeDataListString, FString& OutGptRequestString)
{
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();
	
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

	return FJsonObjectConverter::UStructToJsonObjectString(GptRequest, OutGptRequestString, 0, 0);
}

FString FCommentGenerationUtility::GetDesiredPrompt(const FString& NodeDataListString)
{
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();
	
	FString Prompt = TEXT("You are developing a game using the Unreal Engine and are going to write a comment in a comment node for a blueprint process represented by the following string in JSON format. Answer the appropriate comment to be written in the comment node according to the following conditions.");

	Prompt += TEXT("\n\n- answer in ") + TsubasamusuUnrealAssistSettings->GetGptLanguageCulture()->GetEnglishName();

	for (FString CommentGenerationCondition : TsubasamusuUnrealAssistSettings->CommentGenerationConditions)
	{
		Prompt += TEXT("\n- ") + CommentGenerationCondition;
	}

	Prompt += TEXT("\n\n") + NodeDataListString;

	return Prompt;
}

TArray<UEdGraphNode*> FCommentGenerationUtility::GetNodesUnderComment(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
	//TODO: コメントノード内のノードの更新
	//「HandleSelection(true, true)」を「SGraphNodeComment::OnMouseButtonUp()」経由で間接的に呼び出すか？

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

bool FCommentGenerationUtility::TryGetNodeDataListStringUnderComment(FString& OutNodeDataListString, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
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

	const FNodeDataList NodeDataList = GetNodeDataList(ActiveNodesUnderComment);
	return FJsonObjectConverter::UStructToJsonObjectString(NodeDataList, OutNodeDataListString, 0, 0);
}

#undef LOCTEXT_NAMESPACE
