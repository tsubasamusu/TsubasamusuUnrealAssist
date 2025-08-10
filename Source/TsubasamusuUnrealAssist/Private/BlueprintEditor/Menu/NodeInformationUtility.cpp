// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "BlueprintEditor/Menu/NodeInformation/NodeInformationUtility.h"
#include "EdGraphNode_Comment.h"
#include "JsonObjectConverter.h"
#include "BlueprintEditor/Menu/NodeInformation/NodeData.h"

bool FNodeInformationUtility::TryGetNodeDataListString(FString& OutNodeDataListString, const TArray<UEdGraphNode*>& InNodes)
{
	const FNodeDataList NodeDataList = GetNodeDataList(InNodes);
	return FJsonObjectConverter::UStructToJsonObjectString(NodeDataList, OutNodeDataListString, 0, 0);
}

FNodeDataList FNodeInformationUtility::GetNodeDataList(const TArray<UEdGraphNode*>& InNodes)
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

TArray<FPinData> FNodeInformationUtility::GetPinDataList(const UEdGraphNode* InNode)
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
			.bThisPinUsesDefaultValue = IsPinUsesDefaultValue(Pin),
			.ConnectedPinIds = GetPinIds(Pin->LinkedTo)
		};

		PinDataList.Add(PinData);
	}

	return PinDataList;
}

TArray<FString> FNodeInformationUtility::GetPinIds(const TArray<UEdGraphPin*>& InPins)
{
	TArray<FString> PinIds;

	for (const UEdGraphPin* Pin : InPins)
	{
		PinIds.Add(Pin->PinId.ToString());
	}

	return PinIds;
}

FString FNodeInformationUtility::GetPinTypeAsString(const UEdGraphPin* InPin)
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

bool FNodeInformationUtility::IsPinUsesDefaultValue(const UEdGraphPin* InPin)
{
	return !InPin->HasAnyConnections() && InPin->Direction == EGPD_Input;
}

bool FNodeInformationUtility::IsCommentNode(const UEdGraphNode* InNode)
{
	return IsValid(Cast<UEdGraphNode_Comment>(InNode));
}
