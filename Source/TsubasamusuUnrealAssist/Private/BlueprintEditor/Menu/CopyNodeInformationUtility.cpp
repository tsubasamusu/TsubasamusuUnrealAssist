// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "BlueprintEditor/Menu/CopyNodeInformationUtility.h"
#include  "BlueprintEditor/GraphNodeUtility.h"
#include "BlueprintEditor/Menu/NodeInformationUtility.h"
#include "Windows/WindowsPlatformApplicationMisc.h"

#define LOCTEXT_NAMESPACE "FCopyNodeInformationUtility"

void FCopyNodeInformationUtility::AddCopyNodeInformationMenu(const TWeakObjectPtr<UEdGraph> InGraph, FMenuBuilder& InMenuBuilder)
{
	const FName ExtensionHookName = TEXT("TsubasamusuUnrealAssistSection");
	const TAttribute HeadingText = LOCTEXT("TsubasamusuUnrealAssistSectionHeader", "Tsubasamusu Unreal Assist");
    
	InMenuBuilder.BeginSection(ExtensionHookName, HeadingText);

	const FText LabelText = LOCTEXT("CreateArrayNodeLabelText", "Copy in JSON format");
	const FText ToolTipText = LOCTEXT("CreateArrayNodeToolTipText", "Copy selected nodes in JSON format.");

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
