// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "Blueprint/NodeInformationCopier.h"
#include "Blueprint/NodeInformationUtility.h"
#include "Type/TsubasamusuUnrealAssistStructs.h"
#include "Windows/WindowsPlatformApplicationMisc.h"

#define LOCTEXT_NAMESPACE "FNodeInformationCopier"

FSelectedNodeMenuContext FNodeInformationCopier::CreateSelectedNodeMenuContext()
{
	const FText JsonMenuLabelText = LOCTEXT("CopyNodeInformationInJsonFormatLabel", "JSON");
	
	const FOnSelectedNodeSubMenuClicked OnClickedSubMenu = [JsonMenuLabelText](const TArray<TWeakObjectPtr<UEdGraphNode>>& InSelectedNodes, const TWeakObjectPtr<UEdGraph>, const FText& InSubMenuLabelText)
	{
		FString NodeDataListString;
		
		const bool bSuccessfullyGotNodeDataListString = InSubMenuLabelText.EqualTo(JsonMenuLabelText) ?
			FNodeInformationUtility::TryGetNodeDataListString(NodeDataListString, InSelectedNodes) :
			FNodeInformationUtility::TryGetNodeDataListToonString(NodeDataListString, InSelectedNodes);

		if (bSuccessfullyGotNodeDataListString)
		{
			FPlatformApplicationMisc::ClipboardCopy(*NodeDataListString);
		}
	};
	
	FSelectedNodeSubMenuContext JsonSubMenuContext
	{
		.OnClicked = OnClickedSubMenu,
		.LabelText = JsonMenuLabelText,
		.ToolTipText = LOCTEXT("CopyNodeInformationInJsonFormatToolTip", "Copy selected nodes in JSON Format.")
	};
	
	FSelectedNodeSubMenuContext ToonSubMenuContext
	{
		.OnClicked = OnClickedSubMenu,
		.LabelText = LOCTEXT("CopyNodeInformationInToonFormatLabel", "TOON"),
		.ToolTipText = LOCTEXT("CopyNodeInformationInToonFormatToolTip", "Copy selected nodes in TOON Format.")
	};
	
	const FShouldAddMenu ShouldAddMenu = [](const TArray<TWeakObjectPtr<UEdGraphNode>>& InSelectedNodes)
	{
		return !InSelectedNodes.IsEmpty();
	};
	
	return FSelectedNodeMenuContext
	{
		.ShouldAddMenu = ShouldAddMenu,
		.LabelText = LOCTEXT("CopyNodeInformationLabel", "Copy as..."),
		.ToolTipText = LOCTEXT("CopyNodeInformationToolTip", "Copy selected nodes as string."),
		.SubMenuContexts = { JsonSubMenuContext, ToonSubMenuContext }
	};
}

#undef LOCTEXT_NAMESPACE
