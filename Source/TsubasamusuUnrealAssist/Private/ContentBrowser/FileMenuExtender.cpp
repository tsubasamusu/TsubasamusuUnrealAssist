// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "ContentBrowser/FileMenuExtender.h"
#include "ContentBrowserDataMenuContexts.h"
#include "ContentBrowserAssetDataCore.h"
#include "ContentBrowserAssetDataPayload.h"

#define LOCTEXT_NAMESPACE "FFileMenuExtender"

void FFileMenuExtender::RegisterFileMenu()
{
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateStatic(&AddFileMenu));
}

void FFileMenuExtender::AddFileMenu()
{
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu");
	check(IsValid(Menu));
	
	Menu->AddDynamicSection(TEXT("DynamicSection_StormSync_ContextMenu_Asset"), FNewToolMenuDelegate::CreateLambda([](UToolMenu* InMenu)
	{
		const UContentBrowserDataMenuContext_FileMenu* FileMenuContext = InMenu->FindContext<UContentBrowserDataMenuContext_FileMenu>();
		check(IsValid(FileMenuContext));

		const bool bCanBeModified = !FileMenuContext || FileMenuContext->bCanBeModified;
		
		if (!bCanBeModified)
		{
			return;
		}

		const FName SectionName = TEXT("TsubasamusuUnrealAssistSection");
		const FText SectionLabelText = LOCTEXT("TsubasamusuUnrealAssistSectionLabelText", "Tsubasamusu Unreal Assist");
		
		FToolMenuSection& Section = InMenu->AddSection(SectionName, SectionLabelText);
		Section.InsertPosition = FToolMenuInsert(TEXT("AssetContextReferences"), EToolMenuInsertType::Before);

		const FName MenuName = TEXT("ZipAssetMenu");
		const FText MenuLabelText = LOCTEXT("ZipAssetMenuLabelText", "ZIP Asset(s)");
		const FText MenuToolTipText = LOCTEXT("ZipAssetMenuTooltip", "Compress the selected asset(s) into a ZIP file.");

		const FSlateIcon MenuIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "MainFrame.ZipUpProject");
		const FUIAction MenuAction = FUIAction(FExecuteAction::CreateStatic(&Hoge));
		
		Section.AddMenuEntry(MenuName, MenuLabelText, MenuToolTipText, MenuIcon, MenuAction);
	}));
}

void FFileMenuExtender::GetSelectedAssetPackageNames(const UToolMenu* InMenu, TArray<FString>& OutSelectedAssetPackageNames)
{
	check(IsValid(InMenu));

	const UContentBrowserDataMenuContext_FileMenu* FileMenuContext = InMenu->FindContext<UContentBrowserDataMenuContext_FileMenu>();
	check(IsValid(FileMenuContext));

	for (const FContentBrowserItem& SelectedItem : FileMenuContext->SelectedItems)
	{
		const FContentBrowserItemData* SelectedItemData = SelectedItem.GetPrimaryInternalItem();
		
		if (!SelectedItemData)
		{
			continue;
		}

		const UContentBrowserDataSource* SelectedItemDataSource = SelectedItemData->GetOwnerDataSource();
		
		if (!SelectedItemDataSource)
		{
			continue;
		}

		for (const FContentBrowserItemData& InternalItemData : SelectedItem.GetInternalItems())
		{
			if (const TSharedPtr<const FContentBrowserAssetFileItemDataPayload> ItemPayload = ContentBrowserAssetData::GetAssetFileItemPayload(SelectedItemDataSource, InternalItemData))
			{
				OutSelectedAssetPackageNames.Add(ItemPayload->GetAssetData().PackageName.ToString());
			}
		}
	}
}

void FFileMenuExtender::GetSelectedAssetPackageNames(const UToolMenu* InMenu, TArray<FName>& OutSelectedAssetPackageNames)
{
	TArray<FString> SelectedAssetPackageNamesAsString;
	GetSelectedAssetPackageNames(InMenu, SelectedAssetPackageNamesAsString);

	TArray<FName> SelectedAssetPackageNames;
	Algo::Transform(SelectedAssetPackageNamesAsString, SelectedAssetPackageNames, [](const FString& SelectedAssetPackageNameAsString)
	{
		return FName(*SelectedAssetPackageNameAsString);
	});

	OutSelectedAssetPackageNames = MoveTemp(SelectedAssetPackageNames);
}

void FFileMenuExtender::Hoge()
{
	
}

#undef LOCTEXT_NAMESPACE
