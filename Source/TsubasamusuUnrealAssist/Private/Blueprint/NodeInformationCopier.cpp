// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "Blueprint/NodeInformationCopier.h"
#include "Blueprint/NodeInformationUtility.h"
#include "Type/TsubasamusuUnrealAssistStructs.h"
#include "Type/TsubasamusuUnrealAssistMacros.h"
#include "Windows/WindowsPlatformApplicationMisc.h"

#define LOCTEXT_NAMESPACE "FNodeInformationCopier"

FSelectedNodeMenuContext FNodeInformationCopier::CreateSelectedNodeMenuContext()
{
	const FText JsonMenuLabelText = LOCTEXT("CopyNodeInformationInJsonFormatLabel", "JSON");
	
	const FOnSelectedNodeSubMenuClicked OnSubMenuClicked = [JsonMenuLabelText](const TArray<TWeakObjectPtr<UEdGraphNode>>& InSelectedNodes, const TWeakObjectPtr<UEdGraph>, const FText& InSubMenuLabelText)
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
	
	const FShouldAddMenu ShouldAddMenu = [](const TArray<TWeakObjectPtr<UEdGraphNode>>& InSelectedNodes)
	{
		return !InSelectedNodes.IsEmpty();
	};
	
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 3, 0)
	FSelectedNodeSubMenuContext JsonSubMenuContext
	{
		.OnClicked = OnSubMenuClicked,
		.LabelText = JsonMenuLabelText,
		.ToolTipText = LOCTEXT("CopyNodeInformationInJsonFormatToolTip", "Copy selected nodes in JSON Format.")
	};
	
	FSelectedNodeSubMenuContext ToonSubMenuContext
	{
		.OnClicked = OnSubMenuClicked,
		.LabelText = LOCTEXT("CopyNodeInformationInToonFormatLabel", "TOON"),
		.ToolTipText = LOCTEXT("CopyNodeInformationInToonFormatToolTip", "Copy selected nodes in TOON Format.")
	};
	
	return FSelectedNodeMenuContext
	{
		.ShouldAddMenu = ShouldAddMenu,
		.LabelText = LOCTEXT("CopyNodeInformationLabel", "Copy as..."),
		.ToolTipText = LOCTEXT("CopyNodeInformationToolTip", "Copy selected nodes as string."),
		.SubMenuContexts = { JsonSubMenuContext, ToonSubMenuContext }
	};
#else
	FSelectedNodeSubMenuContext JsonSubMenuContext;
	
	JsonSubMenuContext.OnClicked = OnSubMenuClicked;
	JsonSubMenuContext.LabelText = JsonMenuLabelText;
	JsonSubMenuContext.ToolTipText = LOCTEXT("CopyNodeInformationInJsonFormatToolTip", "Copy selected nodes in JSON Format.");
	
	FSelectedNodeSubMenuContext ToonSubMenuContext;
	
	ToonSubMenuContext.OnClicked = OnSubMenuClicked;
	ToonSubMenuContext.LabelText = LOCTEXT("CopyNodeInformationInToonFormatLabel", "TOON");
	ToonSubMenuContext.ToolTipText = LOCTEXT("CopyNodeInformationInToonFormatToolTip", "Copy selected nodes in TOON Format.");
	
	FSelectedNodeMenuContext SelectedNodeMenuContext;
	
	SelectedNodeMenuContext.ShouldAddMenu = ShouldAddMenu;
	SelectedNodeMenuContext.LabelText = LOCTEXT("CopyNodeInformationLabel", "Copy as...");
	SelectedNodeMenuContext.ToolTipText = LOCTEXT("CopyNodeInformationToolTip", "Copy selected nodes as string.");
	SelectedNodeMenuContext.SubMenuContexts = { JsonSubMenuContext, ToonSubMenuContext };
	
	return SelectedNodeMenuContext;
#endif
}

#undef LOCTEXT_NAMESPACE
