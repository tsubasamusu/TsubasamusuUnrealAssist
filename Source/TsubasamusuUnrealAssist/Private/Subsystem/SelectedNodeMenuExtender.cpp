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
        const TWeakObjectPtr<UEdGraph> WeakGraph = const_cast<UEdGraph*>(InGraph);

        const TArray<TWeakObjectPtr<UEdGraphNode>> SelectedNodes = GetSelectedNodes(InGraph);
        TArray<FSelectedNodeMenuContext> ValidSelectedNodeMenuContexts;
    	
        for (const FSelectedNodeMenuContext& SelectedNodeMenuContext : SelectedNodeMenuContexts)
        {
            if (!SelectedNodes.IsEmpty() && SelectedNodeMenuContext.ShouldAddMenu(SelectedNodes))
            {
                ValidSelectedNodeMenuContexts.Add(SelectedNodeMenuContext);
            }
        }

        if (!ValidSelectedNodeMenuContexts.IsEmpty())
        {
            const FMenuExtensionDelegate AddMainMenuDelegate = FMenuExtensionDelegate::CreateLambda([SelectedNodes, WeakGraph, ValidSelectedNodeMenuContexts](FMenuBuilder& InMainMenuBuilder)
            {
	            if (WeakGraph.IsValid())
	            {
		            const FName ExtensionHookName = TEXT("TsubasamusuUnrealAssist");
					const FText HeadingText = LOCTEXT("TsubasamusuUnrealAssistHeading", "Tsubasamusu Unreal Assist");

					InMainMenuBuilder.BeginSection(ExtensionHookName, HeadingText);

					for (const FSelectedNodeMenuContext& MainMenuContext : ValidSelectedNodeMenuContexts)
					{
						if (MainMenuContext.SubMenuContexts.IsEmpty())
						{
							const FUIAction MainMenuAction = FExecuteAction::CreateLambda([SelectedNodes, WeakGraph, MainMenuContext]()
							{
								MainMenuContext.OnClicked(SelectedNodes, WeakGraph);
							});
							
							InMainMenuBuilder.AddMenuEntry(MainMenuContext.LabelText, MainMenuContext.ToolTipText, MainMenuContext.MenuIcon, MainMenuAction);
						}
						else
						{
							const TArray<FSelectedNodeSubMenuContext> SubMenuContexts = MainMenuContext.SubMenuContexts;
							
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
							
							InMainMenuBuilder.AddSubMenu(MainMenuContext.LabelText, MainMenuContext.ToolTipText, AddSubMenuDelegate, FUIAction(), NAME_None, EUserInterfaceActionType::None, false, MainMenuContext.MenuIcon);
						}
					}

					InMainMenuBuilder.EndSection();
	            }
            });
        	
            const FName AboveSectionName = TEXT("EdGraphSchemaNodeActions");
            Extender->AddMenuExtension(AboveSectionName, EExtensionHook::After, nullptr, AddMainMenuDelegate);
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

TArray<TWeakObjectPtr<UEdGraphNode>> USelectedNodeMenuExtender::GetSelectedNodes(const UEdGraph* InGraph)
{
	TArray<TWeakObjectPtr<UEdGraphNode>> SelectedNodes;

	if (IsValid(InGraph))
	{
		const TSharedPtr<SGraphEditor> GraphEditor = SGraphEditor::FindGraphEditorForGraph(InGraph);

		if (GraphEditor.IsValid())
		{
			const FGraphPanelSelectionSet& SelectedNodeObjects = GraphEditor->GetSelectedNodes();

			for (UObject* SelectedNodeObject : SelectedNodeObjects)
			{
				if (IsValid(SelectedNodeObject))
				{
					TWeakObjectPtr<UEdGraphNode> SelectedNode = Cast<UEdGraphNode>(SelectedNodeObject);
		
					if (SelectedNode.IsValid())
					{
						SelectedNodes.Add(SelectedNode);
					}
				}
			}
		}
	}
	
	return SelectedNodes;
}

#undef LOCTEXT_NAMESPACE
