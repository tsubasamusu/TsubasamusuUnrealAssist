// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "BlueprintEditor/Menu/CommentGeneration/CommentGenerationUtility.h"
#include "EdGraphNode_Comment.h"
#include "BlueprintEditor/Menu/CommentGeneration/NodeData.h"
#include "Setting/TsubasamusuUnrealAssistSettings.h"

#define LOCTEXT_NAMESPACE "CommentGenerationUtility"

void FCommentGenerationUtility::AddCommentGenerationMenu(FMenuBuilder& InMenuBuilder, const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
    const TAttribute LabelText = LOCTEXT("CommentGenerationLabelText", "Generate Comment");
    const TAttribute ToolTipText = LOCTEXT("CommentTranslationToolTipText", "Generate a comment based on the nodes contained in the comment node.");

    InMenuBuilder.AddMenuEntry(LabelText, ToolTipText, FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([InCommentNode]()
    {
    	if (InCommentNode.IsValid())
    	{
    		GenerateComment(InCommentNode);
    	}
    })));
	
    InMenuBuilder.EndSection();
}

void FCommentGenerationUtility::GenerateComment(const TWeakObjectPtr<UEdGraphNode_Comment> InCommentNode)
{
	
}

TArray<FNodeData> FCommentGenerationUtility::GetNodeDataList(const TArray<UEdGraphNode*>& InNodes)
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

	return NodesDataList;
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

#undef LOCTEXT_NAMESPACE
