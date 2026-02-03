// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "NodeUtility/NodeInformationUtility.h"
#include "EdGraphNode_Comment.h"
#include "JsonObjectConverter.h"

bool FNodeInformationUtility::TryGetNodeDataListString(FString& OutNodeDataListString, const TArray<UEdGraphNode*>& InNodes)
{
	const FNodeDataList NodeDataList = GetNodeDataList(InNodes);
	return FJsonObjectConverter::UStructToJsonObjectString(NodeDataList, OutNodeDataListString, 0, 0);
}

bool FNodeInformationUtility::TryGetNodeDataListString(FString& OutNodeDataListString, const TArray<TWeakObjectPtr<UEdGraphNode>>& InWeakNodes)
{
	TArray<UEdGraphNode*> Nodes;
	
	for (TWeakObjectPtr<UEdGraphNode> WeakNode : InWeakNodes)
	{
		Nodes.Add(WeakNode.Get());
	}
	
	return TryGetNodeDataListString(OutNodeDataListString, Nodes);
}

bool FNodeInformationUtility::TryGetNodeDataListToonString(FString& OutNodeDataListString, const TArray<TWeakObjectPtr<UEdGraphNode>>& InWeakNodes)
{
	// まず JSON を取得
	FString JsonString;
	if (!TryGetNodeDataListString(JsonString, InWeakNodes))
	{
		return false;
	}

	// JSON をパース
	TSharedPtr<FJsonObject> RootObj;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (!FJsonSerializer::Deserialize(Reader, RootObj) || !RootObj.IsValid())
	{
		return false;
	}

	// nodeDataList が配列であることを確認
	const TArray<TSharedPtr<FJsonValue>>* NodeArray;
	if (!RootObj->TryGetArrayField(TEXT("nodeDataList"), NodeArray))
	{
		return false;
	}

	// TOON 形式への組み立て
	FString ToonOutput;

	// ヘッダ: 配列名 + 要素数 + フィールド一覧
	{
		int32 Count = NodeArray->Num();
		ToonOutput += FString::Printf(TEXT("nodeDataList[%d]{nodeName,comment,bIsCommentNode,pinDataList}:\n"), Count);
	}

	// 各ノード
	for (const auto& Val : *NodeArray)
	{
		TSharedPtr<FJsonObject> NodeObj = Val->AsObject();
		if (!NodeObj.IsValid())
		{
			continue;
		}

		// ベーシックプロパティ
		FString NodeName = NodeObj->GetStringField(TEXT("nodeName"));
		FString Comment = NodeObj->GetStringField(TEXT("comment"));
		bool bIsComment = NodeObj->GetBoolField(TEXT("bIsCommentNode"));

		ToonOutput += FString::Printf(TEXT("  %s,%s,%s,\n"),
			*NodeName,
			*Comment,
			bIsComment ? TEXT("true") : TEXT("false")
		);

		// pinDataList を取り出し
		const TArray<TSharedPtr<FJsonValue>>* PinArray;
		if (NodeObj->TryGetArrayField(TEXT("pinDataList"), PinArray))
		{
			int32 PinCount = PinArray->Num();

			// pinDataList のヘッダ
			ToonOutput += FString::Printf(
				TEXT("    pinDataList[%d]{pinName,pinDirection,pinType,pinId,defaultValue,bThisPinUsesDefaultValue,connectedPinIds}:\n"),
				PinCount
			);

			// 各 pin
			for (const auto& PinValue : *PinArray)
			{
				TSharedPtr<FJsonObject> PinObj = PinValue->AsObject();
				if (!PinObj.IsValid()) continue;

				// connectedPinIds を文字列に変換
				FString ConnIds = TEXT("");
				const TArray<TSharedPtr<FJsonValue>>* ConnArray;
				if (PinObj->TryGetArrayField(TEXT("connectedPinIds"), ConnArray))
				{
					ConnIds = "[";
					for (int32 i = 0; i < ConnArray->Num(); ++i)
					{
						ConnIds += (*ConnArray)[i]->AsString();
						if (i < ConnArray->Num() - 1)
						{
							ConnIds += TEXT(",");
						}
					}
					ConnIds += TEXT("]");
				}

				// pin の行出力
				ToonOutput += FString::Printf(TEXT("      %s,%s,%s,%s,%s,%s,%s\n"),
					*PinObj->GetStringField(TEXT("pinName")),
					*PinObj->GetStringField(TEXT("pinDirection")),
					*PinObj->GetStringField(TEXT("pinType")),
					*PinObj->GetStringField(TEXT("pinId")),
					*PinObj->GetStringField(TEXT("defaultValue")),
					PinObj->GetBoolField(TEXT("bThisPinUsesDefaultValue")) ? TEXT("true") : TEXT("false"),
					*ConnIds
				);
			}
		}
	}

	OutNodeDataListString = ToonOutput;
	return true;
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
			.PinDirection = GetPinDirectionAsString(Pin),
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
	
	return UEdGraphSchema_K2::GetCategoryText(InPin->PinType.PinCategory, PinSubCategoryName, true).ToString();
}

bool FNodeInformationUtility::IsPinUsesDefaultValue(const UEdGraphPin* InPin)
{
	return !InPin->HasAnyConnections() && InPin->Direction == EGPD_Input;
}

bool FNodeInformationUtility::IsCommentNode(const UEdGraphNode* InNode)
{
	return IsValid(Cast<UEdGraphNode_Comment>(InNode));
}
