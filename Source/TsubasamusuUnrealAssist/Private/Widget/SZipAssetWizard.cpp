// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "Widget/SZipAssetWizard.h"
#include "Interfaces/IMainFrameModule.h"

#define LOCTEXT_NAMESPACE "SZipAssetWizard"

void SZipAssetWizard::Construct(const FArguments& InArgs, const TArray<FName>& InSelectedAssetPackageNames, const TArray<FName>& InFilteredAssetPackageNames, const FOnZipAssetWizardCompleted& InOnZipAssetWizardCompleted)
{
	OnZipAssetWizardCompleted = InOnZipAssetWizardCompleted;
	
	SelectedAssetPackageNames = InSelectedAssetPackageNames;
	FilteredAssetPackageNames = InFilteredAssetPackageNames;
	
	bLastValidityCheckSuccessful = true;
	
	const FText ReportTitleText = LOCTEXT("Zip_ReportTitle", "The following assets will be compress into to a local zip file.");
	
	// ReportPackages = MakeShareable(new TArray<FStormSyncReportPackageData>);
	//
	// for (const FName& PackageName : InPackageNames)
	// {
	// 	ReportPackages.Get()->Add(FStormSyncReportPackageData(PackageName.ToString(), true));
	// }
	//
	// UpdateInputValidity();
	//
	// ChildSlot
	// [
	// 	SNew(SBorder)
	// 	.Padding(18)
	// 	.BorderImage(FAppStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
	// 	[
	// 		SAssignNew(Wizard, SWizard)
	// 		.OnCanceled(this, &SStormSyncExportWizard::OnCancelButtonClicked)
	// 		.OnFinished(this, &SStormSyncExportWizard::OnFinish)
	// 		.CanFinish(this, &SStormSyncExportWizard::CanFinish)
	// 		.ShowPageList(false)
	//
	// 		// Report Dialog page - User choose files to include here
	// 		+SWizard::Page()
	// 		.CanShow(true)
	// 		[
	//
	// 			SAssignNew(ReportDialog, SStormSyncReportDialog, ReportMessage, *ReportPackages.Get())
	// 		]
	//
	// 		// Choose destination page - User choose file destination here (and see the full list of included files)
	// 		+SWizard::Page()
	// 		.CanShow(this, &SStormSyncExportWizard::CanShowDestinationPage)
	// 		.OnEnter(this, &SStormSyncExportWizard::OnDestinationPageEntered)
	// 		[
	// 			SNew(SVerticalBox)
	// 			
	// 			// Title
	// 			+SVerticalBox::Slot()
	// 			.AutoHeight()
	// 			[
	// 				SNew(STextBlock)
	// 				.Font(FAppStyle::Get().GetFontStyle("HeadingExtraSmall"))
	// 				.Text(LOCTEXT("Export_Title_02", "Choose File Path destination"))
	// 				.TransformPolicy(ETextTransformPolicy::ToUpper)
	// 			]
	//
	// 			// Page Description
	// 			+SVerticalBox::Slot()
	// 			.AutoHeight()
	// 			.Padding(0.f, 8.f, 0.f, 4.f)
	// 			[
	// 				SNew(STextBlock)
	// 				.Text(LOCTEXT("DestinationPageDescription_01", "Enter a name for your pak buffer. Pak buffer names may only contain alphanumeric characters, and may not contain a space."))
	// 			]
	//
	// 			+SVerticalBox::Slot()
	// 			.AutoHeight()
	// 			.Padding(0.f, 4.f)
	// 			[
	// 				SNew(STextBlock)
	// 				.Text(LOCTEXT("DestinationPageDescription_02", "When you click the \"Finish\" button below, a pak buffer (.spak) file will be created using this name."))
	// 			]
	//
	// 			+SVerticalBox::Slot()
	// 			.Padding(0.f, 16.f, 0.f, 0.f)
	// 			.AutoHeight()
	// 			[
	// 				SNew(STextBlock)
	// 				.Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
	// 				.Text(LOCTEXT("Export_FileName_Label", "File Name"))
	// 			]
	//
	// 			+SVerticalBox::Slot()
	// 			.Padding(0.f, 8.f, 0.f, 0.f)
	// 			.AutoHeight()
	// 			[
	// 				SAssignNew(FilenameEditBox, SEditableTextBox)
	// 				.Text(this, &SStormSyncExportWizard::GetFileNameText)
	// 				.OnTextChanged(this, &SStormSyncExportWizard::OnFileNameTextChanged) 
	// 		 	]
	//
	// 			// File Destination
	// 			+SVerticalBox::Slot()
	// 			.Padding(0.f, 16.f, 0.f, 0.f)
	// 			.AutoHeight()
	// 			[
	// 				SNew(STextBlock)
	// 				.Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
	// 				.Text(LOCTEXT("Export_Filepath_Label", "File Path"))
	// 			]
	// 			+SVerticalBox::Slot()
	// 			.Padding(0.f, 8.f, 0.f, 0.f)
	// 			.AutoHeight()
	// 			[
	// 				SNew(SBox)
	// 				.HeightOverride(26.f)
	// 				[
	// 					SNew(SHorizontalBox)
	//
	// 					+SHorizontalBox::Slot()
	// 					.FillWidth(1.0f)
	// 					[
	// 						SNew(SEditableTextBox)
	// 						.IsEnabled(false)
	// 						.Text(this, &SStormSyncExportWizard::GetFilePathText)
	// 						.OnTextChanged(this, &SStormSyncExportWizard::OnFilePathTextChanged)
	// 					]
	//
	// 					+SHorizontalBox::Slot()
	// 					.AutoWidth()
	// 					.Padding(6.0f, 1.0f, 0.0f, 0.0f)
	// 					[
	// 						SNew(SButton)
	// 						.VAlign(VAlign_Center)
	// 						.ButtonStyle(FAppStyle::Get(), "SimpleButton")
	// 						.OnClicked(this, &SStormSyncExportWizard::HandleChooseFolderButtonClicked)
	// 						[
	// 							SNew(SImage)
	// 							.Image(FAppStyle::Get().GetBrush("Icons.FolderClosed"))
	// 							.ColorAndOpacity(FSlateColor::UseForeground())
	// 						]
	// 					]
	// 				]
	// 			]
	//
	// 			// File List
	// 			+SVerticalBox::Slot()
	// 			.Padding(0.f, 16.f, 0.f, 0.f)
	// 			.AutoHeight()
	// 			[
	// 				SNew(STextBlock)
	// 				.Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
	// 				.Text(LOCTEXT("Export_ListOfFiles_Label", "List of files to include"))
	// 			]
	//
	// 			+SVerticalBox::Slot()
	// 			.AutoHeight()
	// 			.Padding(0.f, 8.f, 0.f, 4.f)
	// 			[
	// 				SNew(STextBlock)
	// 				.Text(LOCTEXT("Export_ListOfFiles_Description", "Here is the list of files to be included in the pak buffer. You can go back to the previous step with the \"Back\" button below to edit this list."))
	// 			]
	//
	// 			+SVerticalBox::Slot()
	// 			.FillHeight(1.f)
	// 			.Padding(0.f, 8.f)
	// 			[
	// 				SAssignNew(FileDependenciesListView, SListView<TSharedPtr<FStormSyncFileDependency>>)
	// 				.ListItemsSource(&FileDependencyList)
	// 				.HeaderRow(SNew(SHeaderRow)
	// 					+SHeaderRow::Column(StormSync::SlateWidgetRow::HeaderRow_PackageName)
	// 					.DefaultLabel(StormSync::SlateWidgetRow::DefaultLabel_PackageName)
	// 					.SortMode(this, &SStormSyncExportWizard::GetSortModeForColumn, StormSync::SlateWidgetRow::HeaderRow_PackageName)
	// 					.OnSort(this, &SStormSyncExportWizard::OnSortAttributeEntries)
	// 					.FillWidth(0.4f)
	// 					+SHeaderRow::Column(StormSync::SlateWidgetRow::HeaderRow_FileSize)
	// 					.DefaultLabel(StormSync::SlateWidgetRow::DefaultLabel_FileSize)
	// 					.SortMode(this, &SStormSyncExportWizard::GetSortModeForColumn, StormSync::SlateWidgetRow::HeaderRow_FileSize)
	// 					.OnSort(this, &SStormSyncExportWizard::OnSortAttributeEntries)
	// 					.FillWidth(0.1f)
	// 					+SHeaderRow::Column(StormSync::SlateWidgetRow::HeaderRow_Timestamp)
	// 					.DefaultLabel(StormSync::SlateWidgetRow::DefaultLabel_Timestamp)
	// 					.SortMode(this, &SStormSyncExportWizard::GetSortModeForColumn, StormSync::SlateWidgetRow::HeaderRow_Timestamp)
	// 					.OnSort(this, &SStormSyncExportWizard::OnSortAttributeEntries)
	// 					.FillWidth(0.2f)
	// 					+SHeaderRow::Column(StormSync::SlateWidgetRow::HeaderRow_FileHash)
	// 					.DefaultLabel(StormSync::SlateWidgetRow::DefaultLabel_FileHash)
	// 					.SortMode(this, &SStormSyncExportWizard::GetSortModeForColumn, StormSync::SlateWidgetRow::HeaderRow_FileHash)
	// 					.OnSort(this, &SStormSyncExportWizard::OnSortAttributeEntries)
	// 					.FillWidth(0.2f)
	// 				)
	// 				.OnGenerateRow(this, &SStormSyncExportWizard::MakeFileDependencyWidget)
	// 			]
	//
	// 			+SVerticalBox::Slot()
	// 			.AutoHeight()
	// 			[
	// 				SNew(STextBlock)
	// 				.Text(this, &SStormSyncExportWizard::GetExpectedFileSize)
	// 			]
	// 			
	// 			// Error validation
	// 			+SVerticalBox::Slot()
	// 			.Padding(0.f, 16.f)
	// 			.AutoHeight()
	// 			[
	// 				SNew(SWarningOrErrorBox)
	// 				.MessageStyle(EMessageStyle::Error)
	// 				.Visibility(this, &SStormSyncExportWizard::GetNameErrorLabelVisibility)
	// 				.Message(this, &SStormSyncExportWizard::GetNameErrorLabelText)
	// 			]
	// 		]
	// 	]
	// ];
}

void SZipAssetWizard::OpenWizard(const TArray<FName>& InSelectedAssetPackageNames, const TArray<FName>& InFilteredAssetPackageNames, const FOnZipAssetWizardCompleted& InOnZipAssetWizardCompleted)
{
	const TSharedRef<SWindow> ReportWindow = SNew(SWindow)
		.Title(LOCTEXT("Window_Title", "Tsubasamusu Unreal Assist | Zip the selected asset(s)"))
		.ClientSize(FVector2D(960, 700))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		[
			SNew(SZipAssetWizard, InSelectedAssetPackageNames, InFilteredAssetPackageNames, InOnZipAssetWizardCompleted)
		];

	const IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	const TSharedPtr<SWindow> ParentWindow = MainFrameModule.GetParentWindow();
	
	if (ParentWindow.IsValid())
	{
		FSlateApplication::Get().AddWindowAsNativeChild(ReportWindow, MainFrameModule.GetParentWindow().ToSharedRef());
	}
	else
	{
		FSlateApplication::Get().AddWindow(ReportWindow);
	}
}

#undef LOCTEXT_NAMESPACE
