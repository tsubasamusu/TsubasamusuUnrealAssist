// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "Blueprint/NodeInformationCopier.h"
#include "Blueprint/NodeUtility.h"
#include "Blueprint/NodeInformationUtility.h"
#include "Windows/WindowsPlatformApplicationMisc.h"

#define LOCTEXT_NAMESPACE "TsubasamusuUnrealAssist"

void FNodeInformationCopier::AddCopyNodeInformationMenu(const TWeakObjectPtr<UEdGraph> InGraph, FMenuBuilder& InMenuBuilder)
{
	if (!InGraph.IsValid())
	{
		return;
	}
	
	const FName ExtensionHookName = TEXT("TsubasamusuUnrealAssistSection");
	const TAttribute<FText> HeadingText = LOCTEXT("CopyNodeInformationHeading", "Tsubasamusu Unreal Assist");
    
	InMenuBuilder.BeginSection(ExtensionHookName, HeadingText);

	const FText MainMenuLabelText = LOCTEXT("CopyNodeInformationLabel", "Copy as...");
	const FText MainMenuToolTipText = LOCTEXT("CopyNodeInformationToolTip", "Copy selected nodes as string.");

	const FSlateIcon MainMenuIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "GenericCommands.Copy");

	const TArray<TWeakObjectPtr<UEdGraphNode>> SelectedWeakNodes = FNodeUtility::GetSelectedWeakNodes(InGraph.Get());
	const auto MenuAction = [SelectedWeakNodes](FMenuBuilder& MenuBuilder)
	{
		AddCopyNodeInformationSubMenus(MenuBuilder, SelectedWeakNodes);
	};
    
	InMenuBuilder.AddSubMenu(MainMenuLabelText, MainMenuToolTipText, FNewMenuDelegate::CreateLambda(MenuAction), FUIAction(), NAME_None, EUserInterfaceActionType::None, false, MainMenuIcon);

	InMenuBuilder.EndSection();
}

void FNodeInformationCopier::AddCopyNodeInformationSubMenus(FMenuBuilder& InMenuBuilder, const TArray<TWeakObjectPtr<UEdGraphNode>>& InSelectedNodes)
{
	// JSON
	{
		const TAttribute<FText> CopyNodeInformationInJsonFormatLabelText = LOCTEXT("CopyNodeInformationInJsonFormatLabel", "JSON");
		const TAttribute<FText> CopyNodeInformationInJsonFormatToolTipText = LOCTEXT("CopyNodeInformationInJsonFormatToolTip", "Copy selected nodes in JSON Format.");

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
		const TAttribute<FText> CopyNodeInformationInToonFormatLabelText = LOCTEXT("CopyNodeInformationInToonFormatLabel", "TOON");
		const TAttribute<FText> CopyNodeInformationInToonFormatToolTipText = LOCTEXT("CopyNodeInformationInToonFormatToolTip", "Copy selected nodes in TOON Format.");

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
