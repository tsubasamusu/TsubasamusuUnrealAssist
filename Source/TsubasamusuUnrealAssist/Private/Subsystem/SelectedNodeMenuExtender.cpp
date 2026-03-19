// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "SelectedNodeMenuExtender.h"
#include "Blueprint/NodeUtility.h"
#include "Type/TsubasamusuUnrealAssistStructs.h"

#define LOCTEXT_NAMESPACE "SelectedNodeMenuExtender"

void USelectedNodeMenuExtender::Deinitialize()
{
	UnregisterSelectedNodeMenu();
}

void USelectedNodeMenuExtender::AddSelectedNodeMenu(const FSelectedNodeMenuContext& InSelectedNodeMenuContext)
{
	UnregisterSelectedNodeMenu();
	SelectedNodeMenuContexts.Add(InSelectedNodeMenuContext);
	RegisterSelectedNodeMenu();
}

void USelectedNodeMenuExtender::RegisterSelectedNodeMenu()
{
	FGraphEditorModule& GraphEditorModule = FModuleManager::LoadModuleChecked<FGraphEditorModule>(TEXT("GraphEditor"));
	TArray<FExtendSelectedNodeMenuDelegate>& ExtendSelectedNodeMenuDelegates = GraphEditorModule.GetAllGraphEditorContextMenuExtender();
	
	const bool bAlreadyRegistered = ExtendSelectedNodeMenuDelegates.ContainsByPredicate([this](const FExtendSelectedNodeMenuDelegate& InExtendSelectedNodeMenuDelegate)
	{
		return IsMyDelegate(InExtendSelectedNodeMenuDelegate);
	});
	
	if (bAlreadyRegistered)
	{
		check(false);
		return;
	}
	
	ExtendSelectedNodeMenuDelegate = FExtendSelectedNodeMenuDelegate::CreateLambda([this](const TSharedRef<FUICommandList>, const UEdGraph* InGraph, const UEdGraphNode*, const UEdGraphPin*, bool)
	{
		TSharedRef<FExtender> Extender = MakeShared<FExtender>();
		const TArray<TWeakObjectPtr<UEdGraphNode>> SelectedNodes = FNodeUtility::GetSelectedWeakNodes(InGraph);
		
		for (const FSelectedNodeMenuContext& SelectedNodeMenuContext : SelectedNodeMenuContexts)
		{
			if (!SelectedNodes.IsEmpty() && SelectedNodeMenuContext.ShouldAddMenu(SelectedNodes))
			{
				const TWeakObjectPtr<UEdGraph> WeakGraph = const_cast<UEdGraph*>(InGraph);
				
				const FMenuExtensionDelegate AddMainMenuDelegate = FMenuExtensionDelegate::CreateLambda([SelectedNodes, WeakGraph, SelectedNodeMenuContext](FMenuBuilder& InMainMenuBuilder)
				{
					if (WeakGraph.IsValid())
					{
						const FName ExtensionHookName = TEXT("TsubasamusuUnrealAssist");
						const TAttribute<FText> HeadingText = LOCTEXT("TsubasamusuUnrealAssistHeading", "Tsubasamusu Unreal Assist");
					
						InMainMenuBuilder.BeginSection(ExtensionHookName, HeadingText);
					
						if (SelectedNodeMenuContext.SubMenuContexts.IsEmpty())
						{
							const FUIAction MainMenuAction = FExecuteAction::CreateLambda([SelectedNodes, WeakGraph, SelectedNodeMenuContext]()
							{
								SelectedNodeMenuContext.OnClicked(SelectedNodes, WeakGraph);
							});
							
							InMainMenuBuilder.AddMenuEntry(SelectedNodeMenuContext.LabelText, SelectedNodeMenuContext.ToolTipText, SelectedNodeMenuContext.MenuIcon, MainMenuAction);
						}
						else
						{
							const TArray<FSelectedNodeSubMenuContext> SubMenuContexts = SelectedNodeMenuContext.SubMenuContexts;
						
							const FNewMenuDelegate AddSubMenuDelegate = FNewMenuDelegate::CreateLambda([SelectedNodes, WeakGraph, SubMenuContexts](FMenuBuilder& InSubMenuBuilder)
							{
								for (const FSelectedNodeSubMenuContext& SubMenuContext : SubMenuContexts)
								{
									const FUIAction SubMenuAction = FExecuteAction::CreateLambda([SelectedNodes, WeakGraph, SubMenuContext]()
									{
										SubMenuContext.OnClicked(SelectedNodes, WeakGraph, SubMenuContext.LabelText);
									});
						
									InSubMenuBuilder.AddMenuEntry(SubMenuContext.LabelText, SubMenuContext.ToolTipText, FSlateIcon(), SubMenuAction);
								}
							});
						
							InMainMenuBuilder.AddSubMenu(SelectedNodeMenuContext.LabelText, SelectedNodeMenuContext.ToolTipText, AddSubMenuDelegate, FUIAction(), NAME_None, EUserInterfaceActionType::None, false, SelectedNodeMenuContext.MenuIcon);
						}
					
						InMainMenuBuilder.EndSection();
					}
				});
				
				const FName AboveSectionName = TEXT("EdGraphSchemaNodeActions");
				Extender->AddMenuExtension(AboveSectionName, EExtensionHook::After, nullptr, AddMainMenuDelegate);
			}
		}
		
		return Extender;
	});
	
	ExtendSelectedNodeMenuDelegates.Add(ExtendSelectedNodeMenuDelegate);
}

void USelectedNodeMenuExtender::UnregisterSelectedNodeMenu() const
{
	FGraphEditorModule& GraphEditorModule = FModuleManager::LoadModuleChecked<FGraphEditorModule>(TEXT("GraphEditor"));
	TArray<FExtendSelectedNodeMenuDelegate>& ExtendSelectedNodeMenuDelegates = GraphEditorModule.GetAllGraphEditorContextMenuExtender();
	
	ExtendSelectedNodeMenuDelegates.RemoveAll([this](const FExtendSelectedNodeMenuDelegate& InExtendSelectedNodeMenuDelegate)
	{
		return IsMyDelegate(InExtendSelectedNodeMenuDelegate);
	});
}

bool USelectedNodeMenuExtender::IsMyDelegate(const FExtendSelectedNodeMenuDelegate& InExtendSelectedNodeMenuDelegate) const
{
	return InExtendSelectedNodeMenuDelegate.GetHandle() == ExtendSelectedNodeMenuDelegate.GetHandle();
}

#undef LOCTEXT_NAMESPACE
