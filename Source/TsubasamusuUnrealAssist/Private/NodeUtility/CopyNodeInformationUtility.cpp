// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "NodeUtility/CopyNodeInformationUtility.h"
#include  "NodeUtility/GraphNodeUtility.h"
#include "NodeUtility/NodeInformationUtility.h"
#include "Windows/WindowsPlatformApplicationMisc.h"

#define LOCTEXT_NAMESPACE "TsubasamusuUnrealAssist"

void FCopyNodeInformationUtility::AddCopyNodeInformationMenu(const TWeakObjectPtr<UEdGraph> InGraph, FMenuBuilder& InMenuBuilder)
{
	if (!InGraph.IsValid())
	{
		return;
	}
	
	const FName ExtensionHookName = TEXT("TsubasamusuUnrealAssistSection");
	const TAttribute HeadingText = LOCTEXT("CopyNodeInformationHeadingText", "Tsubasamusu Unreal Assist");
    
	InMenuBuilder.BeginSection(ExtensionHookName, HeadingText);

	const FText MainMenuLabelText = LOCTEXT("CopyNodeInformationLabelText", "Copy in...");
	const FText MainMenuToolTipText = LOCTEXT("CopyNodeInformationToolTipText", "Copy selected nodes as string.");

	const FSlateIcon MainMenuIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "GenericCommands.Copy");

	const TArray<TWeakObjectPtr<UEdGraphNode>> SelectedWeakNodes = FGraphNodeUtility::GetSelectedWeakNodes(InGraph.Get());
	const auto MenuAction = [SelectedWeakNodes](FMenuBuilder& MenuBuilder)
	{
		AddCopyNodeInformationSubMenus(MenuBuilder, SelectedWeakNodes);
	};
    
	InMenuBuilder.AddSubMenu(MainMenuLabelText, MainMenuToolTipText, FNewMenuDelegate::CreateLambda(MenuAction), FUIAction(), NAME_None, EUserInterfaceActionType::None, false, MainMenuIcon);

	InMenuBuilder.EndSection();
}

void FCopyNodeInformationUtility::AddCopyNodeInformationSubMenus(FMenuBuilder& InMenuBuilder, const TArray<TWeakObjectPtr<UEdGraphNode>>& InSelectedNodes)
{
	// JSON
	{
		const TAttribute CopyNodeInformationInJsonFormatLabelText = LOCTEXT("CopyNodeInformationInJsonFormatLabelText", "JSON Format");
		const TAttribute CopyNodeInformationInJsonFormatToolTipText = LOCTEXT("CopyNodeInformationInJsonFormatToolTipText", "Copy selected nodes in JSON Format.");

		InMenuBuilder.AddMenuEntry(CopyNodeInformationInJsonFormatLabelText, CopyNodeInformationInJsonFormatToolTipText, FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([InSelectedNodes]()
		{
			FString NodeDataListString;
			if (FNodeInformationUtility::TryGetNodeDataListString(NodeDataListString, InSelectedNodes))
			{
				FPlatformApplicationMisc::ClipboardCopy(*NodeDataListString);
			}
		})));
	}
	
	// TOON
	{
		const TAttribute CopyNodeInformationInToonFormatLabelText = LOCTEXT("CopyNodeInformationInToonFormatLabelText", "TOON Format");
		const TAttribute CopyNodeInformationInToonFormatToolTipText = LOCTEXT("CopyNodeInformationInToonFormatToolTipText", "Copy selected nodes in TOON Format.");

		InMenuBuilder.AddMenuEntry(CopyNodeInformationInToonFormatLabelText, CopyNodeInformationInToonFormatToolTipText, FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([InSelectedNodes]()
		{
			FString NodeDataListToonString;
			if (FNodeInformationUtility::TryGetNodeDataListToonString(NodeDataListToonString, InSelectedNodes))
			{
				FPlatformApplicationMisc::ClipboardCopy(*NodeDataListToonString);
			}
		})));
	}
}

#undef LOCTEXT_NAMESPACE
