// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "NodeUtility/CopyNodeInformationUtility.h"
#include  "NodeUtility/GraphNodeUtility.h"
#include "NodeUtility/NodeInformationUtility.h"
#include "Windows/WindowsPlatformApplicationMisc.h"

#define LOCTEXT_NAMESPACE "TsubasamusuUnrealAssist"

void FCopyNodeInformationUtility::AddCopyNodeInformationMenu(const TWeakObjectPtr<UEdGraph> InGraph, FMenuBuilder& InMenuBuilder)
{
	const FName ExtensionHookName = TEXT("TsubasamusuUnrealAssistSection");
	const TAttribute HeadingText = LOCTEXT("CopyNodeInformationHeadingText", "Tsubasamusu Unreal Assist");
    
	InMenuBuilder.BeginSection(ExtensionHookName, HeadingText);

	const FText LabelText = LOCTEXT("CopyNodeInformationLabelText", "Copy in JSON Format");
	const FText ToolTipText = LOCTEXT("CopyNodeInformationToolTipText", "Copy selected nodes in JSON format.");

	const FSlateIcon MenuIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "GenericCommands.Copy");

	InMenuBuilder.AddMenuEntry(LabelText, ToolTipText, MenuIcon, FUIAction(FExecuteAction::CreateLambda([InGraph]()
	{
		if (InGraph.IsValid())
		{
			FString NodeDataListString;
			const TArray<UEdGraphNode*> SelectedNodes = FGraphNodeUtility::GetSelectedNodes(InGraph.Get());

			if (FNodeInformationUtility::TryGetNodeDataListString(NodeDataListString, SelectedNodes))
			{
				FPlatformApplicationMisc::ClipboardCopy(*NodeDataListString);
			}
		}
	})));
	
	InMenuBuilder.EndSection();
}

#undef LOCTEXT_NAMESPACE
