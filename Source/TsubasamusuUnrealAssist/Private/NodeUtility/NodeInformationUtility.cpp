// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "NodeUtility/NodeInformationUtility.h"
#include "EdGraphNode_Comment.h"
#include "GraphNodeUtility.h"
#include "JsonObjectConverter.h"

bool FNodeInformationUtility::TryGetNodeDataListString(FString& OutNodeDataListString, const TArray<UEdGraphNode*>& InNodes)
{
	const FNodeDataList NodeDataList = GetNodeDataList(InNodes);
	return FJsonObjectConverter::UStructToJsonObjectString(NodeDataList, OutNodeDataListString, 0, 0);
}

bool FNodeInformationUtility::TryGetNodeDataListString(FString& OutNodeDataListString, const TArray<TWeakObjectPtr<UEdGraphNode>>& InWeakNodes)
{
	const TArray<UEdGraphNode*> HardNodes = FGraphNodeUtility::ConvertToHardNodes(InWeakNodes);
	return TryGetNodeDataListString(OutNodeDataListString, HardNodes);
}

bool FNodeInformationUtility::TryGetNodeDataListToonString(FString& OutNodeDataListString, const TArray<UEdGraphNode*>& InNodes)
{
	FString NodeDataListString;
	if (!TryGetNodeDataListString(NodeDataListString, InNodes))
	{
		return false;
	}

	const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(NodeDataListString);
	TSharedPtr<FJsonObject> RootJsonObject;

	if (!FJsonSerializer::Deserialize(JsonReader, RootJsonObject) || !RootJsonObject.IsValid())
	{
		return false;
	}

	const TArray<TSharedPtr<FJsonValue>>* NodesAsJsonValue;
	if (!RootJsonObject->TryGetArrayField(TEXT("nodeDataList"), NodesAsJsonValue))
	{
		return false;
	}

	OutNodeDataListString += FString::Printf(TEXT("nodeDataList[%d]{nodeName,comment,bIsCommentNode,pinDataList}:\n"), NodesAsJsonValue->Num());

	for (const TSharedPtr<FJsonValue>& NodeAsJsonValue : *NodesAsJsonValue)
	{
		TSharedPtr<FJsonObject> NodeAsJsonObject = NodeAsJsonValue->AsObject();
		if (!NodeAsJsonObject.IsValid())
		{
			continue;
		}

		const FString NodeName = NodeAsJsonObject->GetStringField(TEXT("nodeName"));
		const FString Comment = NodeAsJsonObject->GetStringField(TEXT("comment"));
		const bool bIsCommentNode = NodeAsJsonObject->GetBoolField(TEXT("bIsCommentNode"));

		OutNodeDataListString += FString::Printf(TEXT("  %s,%s,%s,\n"), *NodeName, *Comment, bIsCommentNode ? TEXT("true") : TEXT("false"));

		const TArray<TSharedPtr<FJsonValue>>* PinDataListAsJsonValue;
		if (NodeAsJsonObject->TryGetArrayField(TEXT("pinDataList"), PinDataListAsJsonValue))
		{
			OutNodeDataListString += FString::Printf(TEXT("    pinDataList[%d]{pinName,pinDirection,pinType,pinId,defaultValue,bThisPinUsesDefaultValue,connectedPinIds}:\n"), PinDataListAsJsonValue->Num());

			for (const TSharedPtr<FJsonValue>& PinDataAsJsonValue : *PinDataListAsJsonValue)
			{
				TSharedPtr<FJsonObject> PinDataAsJsonObject = PinDataAsJsonValue->AsObject();
				if (!PinDataAsJsonObject.IsValid())
				{
					continue;
				}

				FString ConnectedPinIdsAsString = TEXT("");
				const TArray<TSharedPtr<FJsonValue>>* ConnectedPinIdsAsJsonValue;
				
				if (PinDataAsJsonObject->TryGetArrayField(TEXT("connectedPinIds"), ConnectedPinIdsAsJsonValue))
				{
					ConnectedPinIdsAsString = TEXT("[");
					
					for (int32 i = 0; i < ConnectedPinIdsAsJsonValue->Num(); ++i)
					{
						ConnectedPinIdsAsString += (*ConnectedPinIdsAsJsonValue)[i]->AsString();
						
						if (i < ConnectedPinIdsAsJsonValue->Num() - 1)
						{
							ConnectedPinIdsAsString += TEXT(",");
						}
					}
					
					ConnectedPinIdsAsString += TEXT("]");
				}

				OutNodeDataListString += FString::Printf(TEXT("      %s,%s,%s,%s,%s,%s,%s\n"),
					*PinDataAsJsonObject->GetStringField(TEXT("pinName")),
					*PinDataAsJsonObject->GetStringField(TEXT("pinDirection")),
					*PinDataAsJsonObject->GetStringField(TEXT("pinType")),
					*PinDataAsJsonObject->GetStringField(TEXT("pinId")),
					*PinDataAsJsonObject->GetStringField(TEXT("defaultValue")),
					PinDataAsJsonObject->GetBoolField(TEXT("bThisPinUsesDefaultValue")) ? TEXT("true") : TEXT("false"),
					*ConnectedPinIdsAsString
				);
			}
		}
	}

	return true;
}

bool FNodeInformationUtility::TryGetNodeDataListToonString(FString& OutNodeDataListString, const TArray<TWeakObjectPtr<UEdGraphNode>>& InWeakNodes)
{
	const TArray<UEdGraphNode*> HardNodes = FGraphNodeUtility::ConvertToHardNodes(InWeakNodes);
	return TryGetNodeDataListToonString(OutNodeDataListString, HardNodes);
}

FNodeDataList FNodeInformationUtility::GetNodeDataList(const TArray<UEdGraphNode*>& InNodes)
{
	TArray<FNodeData> NodesDataList;

	for (const UEdGraphNode* Node : InNodes)
	{
#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION <= 2)
		FNodeData NodeData;
		NodeData.NodeName = Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString();
		NodeData.Comment = Node->NodeComment;
		NodeData.bIsCommentNode = IsCommentNode(Node);
		NodeData.PinDataList = GetPinDataList(Node);
#else
		FNodeData NodeData =
		{
			.NodeName = Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString(),
			.Comment = Node->NodeComment,
			.bIsCommentNode = IsCommentNode(Node),
			.PinDataList = GetPinDataList(Node)
		};
#endif
		
		NodesDataList.Add(NodeData);
	}

	return FNodeDataList(NodesDataList);
}

TArray<FPinData> FNodeInformationUtility::GetPinDataList(const UEdGraphNode* InNode)
{
	TArray<FPinData> PinDataList;

	for (const UEdGraphPin* Pin : InNode->GetAllPins())
	{
#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION <= 2)
		FPinData PinData;
		PinData.PinName = Pin->GetDisplayName().IsEmpty() ? Pin->PinName.ToString() : Pin->GetDisplayName().ToString();
		PinData.PinDirection = GetPinDirectionAsString(Pin);
		PinData.PinType = GetPinTypeAsString(Pin);
		PinData.PinId = Pin->PinId.ToString();
		PinData.DefaultValue = Pin->GetDefaultAsString();
		PinData.bThisPinUsesDefaultValue = IsPinUsesDefaultValue(Pin);
		PinData.ConnectedPinIds = GetPinIds(Pin->LinkedTo);
#else
		FPinData PinData
		{
			.PinName = Pin->GetDisplayName().IsEmpty() ? Pin->PinName.ToString() : Pin->GetDisplayName().ToString(),
			.PinDirection = GetPinDirectionAsString(Pin),
			.PinType = GetPinTypeAsString(Pin),
			.PinId = Pin->PinId.ToString(),
			.DefaultValue = Pin->GetDefaultAsString(),
			.bThisPinUsesDefaultValue = IsPinUsesDefaultValue(Pin),
			.ConnectedPinIds = GetPinIds(Pin->LinkedTo)
		};
#endif
		
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

FString FNodeInformationUtility::GetPinDirectionAsString(const UEdGraphPin* InPin)
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

FString FNodeInformationUtility::GetPinTypeAsString(const UEdGraphPin* InPin)
{
	const FName PinSubCategoryName = InPin->PinType.PinSubCategory;
	const UObject* PinSubCategoryObject = InPin->PinType.PinSubCategoryObject.Get();
	
	if (PinSubCategoryName != UEdGraphSchema_K2::PSC_Bitmask && PinSubCategoryObject)
	{
		const UField* PinSubCategoryObjectField = Cast<const UField>(PinSubCategoryObject);
		
		if (IsValid(PinSubCategoryObjectField))
		{
			return PinSubCategoryObjectField->GetDisplayNameText().ToString();
		}
		
		return PinSubCategoryObject->GetName();
	}
	
#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1)
	return UEdGraphSchema_K2::GetCategoryText(InPin->PinType.PinCategory, PinSubCategoryName, true).ToString();
#else
	return UEdGraphSchema_K2::GetCategoryText(InPin->PinType.PinCategory, true).ToString();
#endif
}

bool FNodeInformationUtility::IsPinUsesDefaultValue(const UEdGraphPin* InPin)
{
	return !InPin->HasAnyConnections() && InPin->Direction == EGPD_Input;
}

bool FNodeInformationUtility::IsCommentNode(const UEdGraphNode* InNode)
{
	return IsValid(Cast<UEdGraphNode_Comment>(InNode));
}
