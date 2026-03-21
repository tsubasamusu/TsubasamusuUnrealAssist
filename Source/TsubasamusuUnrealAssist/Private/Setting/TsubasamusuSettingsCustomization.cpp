// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "Setting/TsubasamusuSettingsCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "EditorSettingsUtility.h"
#include "Slate/SLanguageComboButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "RecommendedEditorSettingsApplier.h"
#include "SWarningOrErrorBox.h"
#include "TsubasamusuUnrealAssistSettings.h"
#include "Message/EditorMessageUtility.h"
#include "Subsystem/LlmManager.h"

#define LOCTEXT_NAMESPACE "FTsubasamusuSettingsCustomization"

TSharedRef<IDetailCustomization> FTsubasamusuSettingsCustomization::Create()
{
    return MakeShared<FTsubasamusuSettingsCustomization>();
}

void FTsubasamusuSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& InDetailLayoutBuilder)
{
    AddButtonToApplyRecommendedEditorSettings(InDetailLayoutBuilder);
    AddRestartLlamaServerMessage(InDetailLayoutBuilder);
	AddCommentGenerationLanguageProperty(InDetailLayoutBuilder);
    
    ChangePropertyDisplayAsPassword(InDetailLayoutBuilder, TEXT("Comment Translator"), TEXT("DeeplApiKey"));
}

void FTsubasamusuSettingsCustomization::AddButtonToApplyRecommendedEditorSettings(IDetailLayoutBuilder& InDetailLayoutBuilder)
{
    const FName CategoryName = TEXT("Recommended Editor Settings Applier");
    const FText PropertyText = LOCTEXT("ApplyRecommendedEditorSettingsButtonProperty", "Apply Recommended Editor Settings");
    const FText ButtonText = LOCTEXT("ApplyRecommendedEditorSettingsButtonLabel", "Apply");
    const FText ToolTipText = LOCTEXT("ApplyRecommendedEditorSettingsButtonToolTip",
        "Apply the editor settings recommended by this plugin’s developer. Specifically, as follows:\n\n"
        "- Disable auto save.\n"
        "- Do not restore the tabs of assets that were open previously when the editor is restarted.\n"
        "- Set the location where asset editors open to “Main Window.”\n"
        "- Make comment bubble visible when zooming out in the Event Graph.\n"
        "- Set the default color of comment nodes to black.\n"
        "- Set the editor language to English.\n"
        "- Also use English for node names, pin names, and property names.\n"
        "- Prevent nodes that are placed in a “ghost” state by default, such as BeginPlay and Tick, from being placed by default. (So that no “ghost” nodes exist when creating a Blueprint.)\n"
        "- Make Cast nodes appear as Pure by default, with no execution pins.\n"
        "- Display the Output Log in color.\n"
        "- Make the toolbar icons display more compactly.\n"  
        "- When starting PIE, automatically focus the viewport even if it hasn’t been clicked."
    );
    
    IDetailCategoryBuilder& DetailCategoryBuilder = InDetailLayoutBuilder.EditCategory(CategoryName);
    
    DetailCategoryBuilder.AddCustomRow(PropertyText)
    .NameContent()
    [
        SNew(STextBlock)
        .Text(PropertyText)
        .Font(InDetailLayoutBuilder.GetDetailFont())
        .ToolTipText(ToolTipText)
    ]
    .ValueContent()
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        [
            SNew(SButton)
            .Text(ButtonText)
            .HAlign(HAlign_Center)
            .ToolTipText(ToolTipText)
            .OnClicked_Lambda([]()
            {
                const FText WarningMessage = LOCTEXT("ApplyRecommendedEditorSettingsWarningMessage", "Are you sure you want to change editor settings?");
                
                if (FEditorMessageUtility::OpenWarningMessageDialog(EAppMsgType::YesNo, WarningMessage))
                {
                    FRecommendedEditorSettingsApplier::ApplyRecommendedEditorSettings();
                }
                
                return FReply::Handled();
            })
        ]
    ];
}

void FTsubasamusuSettingsCustomization::AddRestartLlamaServerMessage(IDetailLayoutBuilder& InDetailLayoutBuilder)
{
    const FName CategoryName = TEXT("LLM");
    const FText RowFilterText = LOCTEXT("RestartLlamaServerRowFilter", "Restart Llama Server");
    
    IDetailCategoryBuilder& DetailCategoryBuilder = InDetailLayoutBuilder.EditCategory(CategoryName);

    DetailCategoryBuilder.AddCustomRow(RowFilterText)
    .Visibility(TAttribute<EVisibility>::CreateStatic(&GetRestartLlamaServerMessageVisibility))
    .WholeRowWidget
    [
        SNew(SBox)
        .Padding(0.f, 5.f)
        [
            SNew(SWarningOrErrorBox)
            .MessageStyle_Static(&GetRestartLlamaServerMessageStyle)
            .Message_Static(&GetRestartLlamaServerMessageText)
            [
                SNew(SButton)
                .Text_Static(&GetRestartLlamaServerButtonText)
                .OnClicked_Lambda([]()
                {
                    ULlmManager::GetChecked()->RestartLlamaServer();
                    return FReply::Handled();
                })
            ]
        ]
    ];
}

void FTsubasamusuSettingsCustomization::AddCommentGenerationLanguageProperty(IDetailLayoutBuilder& InDetailLayoutBuilder)
{
    const FName CategoryName = TEXT("Comment Generator");
    const FText CategoryText = LOCTEXT("CommentGenerationCategory", "Comment Generator");
    const FText PropertyText = LOCTEXT("CommentGenerationLanguageProperty", "Language");
    const FText ToolTipText = LOCTEXT("CommentGenerationLanguageToolTip", "The language of comments generated by LLM. (This property will become editable if you uncheck \"Use Editor Language.\")");

    IDetailCategoryBuilder& DetailCategoryBuilder = InDetailLayoutBuilder.EditCategory(CategoryName);
    
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 4, 0)
    check(!DetailCategoryBuilder.IsEmpty());
#endif
    
    TSharedPtr<IPropertyHandle> UseEditorLanguagePropertyHandle = InDetailLayoutBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UTsubasamusuUnrealAssistSettings, bUseEditorLanguageForCommentGeneration));
    
    if (UseEditorLanguagePropertyHandle.IsValid())
    {
        DetailCategoryBuilder.AddProperty(UseEditorLanguagePropertyHandle);
    }
    
    const TSharedRef<FLocalizedCulturesFlyweight> LocalizedCulturesFlyweight = MakeShared<FLocalizedCulturesFlyweight>();

    DetailCategoryBuilder.AddCustomRow(PropertyText)
    .NameContent()
    [
        SNew(STextBlock)
        .Text(PropertyText)
        .Font(InDetailLayoutBuilder.GetDetailFont())
        .ToolTipText(ToolTipText)
    ]
    .ValueContent()
    [
        SNew(SLanguageComboButton, LocalizedCulturesFlyweight)
        .OnLanguageSelected_Lambda([](FCulturePtr InSelectedLanguageCulture)
        {
            UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
            TsubasamusuUnrealAssistSettings->SetCommentGenerationLanguageCulture(InSelectedLanguageCulture);
        })
        .IsEnabled_Lambda([]()
        {
	        const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
            return !TsubasamusuUnrealAssistSettings->bUseEditorLanguageForCommentGeneration;
        })
    ];
}

EVisibility FTsubasamusuSettingsCustomization::GetRestartLlamaServerMessageVisibility()
{
    const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
    const ULlmManager* LlmManager = ULlmManager::GetChecked();
    
    const bool bAppliedCurrentLlamaServerSettings = TsubasamusuUnrealAssistSettings->GetCurrentLlamaServerSettings() == LlmManager->GetLastAppliedLlamaServerSettings();
    const bool bLlamaServerIsAlreadyRunning = LlmManager->GetLlamaServerStatus() == ELlamaServerStatus::SuccessfullyStarted;
    
    return bAppliedCurrentLlamaServerSettings && bLlamaServerIsAlreadyRunning ? EVisibility::Collapsed : EVisibility::Visible;
}

EMessageStyle FTsubasamusuSettingsCustomization::GetRestartLlamaServerMessageStyle()
{
    const ELlamaServerStatus LlamaServerStatus = ULlmManager::GetChecked()->GetLlamaServerStatus();
    
    if (LlamaServerStatus == ELlamaServerStatus::FailedToStart)
    {
        return EMessageStyle::Error;
    }
    
    return EMessageStyle::Warning;
}

FText FTsubasamusuSettingsCustomization::GetRestartLlamaServerButtonText()
{
    const FText StartText = LOCTEXT("StartLlamaServerButtonLabel", "Start Llama Server");
    const FText RestartText = LOCTEXT("RestartLlamaServerButtonLabel", "Restart Llama Server");
    
    const ELlamaServerStatus LlamaServerStatus = ULlmManager::GetChecked()->GetLlamaServerStatus();
    return LlamaServerStatus == ELlamaServerStatus::SuccessfullyStarted ? RestartText : StartText;
}

FText FTsubasamusuSettingsCustomization::GetRestartLlamaServerMessageText()
{
    FText MessageText;
    const ELlamaServerStatus LlamaServerStatus = ULlmManager::GetChecked()->GetLlamaServerStatus();
    
    switch (LlamaServerStatus)
    {
    case ELlamaServerStatus::NotStartedYet:
        MessageText = LOCTEXT("StartLlamaServerMessage", "Llama server has not yet started. You must start the server to use AI-related features.");
        break;
    case ELlamaServerStatus::SuccessfullyStarted:
        MessageText = LOCTEXT("RestartLlamaServerMessage", "Llama server settings have been updated. Please restart the server to apply these changes.");
        break;
    case ELlamaServerStatus::FailedToStart:
        MessageText = LOCTEXT("FailedToStartLlamaServerMessage", "Llama server failed to start. The server is currently not running. Please check your settings and try starting the server again. For more details, see Output Log.");
        break;
    default:
        checkNoEntry();
    }
    
    return MessageText;
}

void FTsubasamusuSettingsCustomization::ChangePropertyDisplayAsPassword(IDetailLayoutBuilder& InDetailLayoutBuilder, const FName& InCategoryName, const FName& InPropertyName)
{
    IDetailCategoryBuilder& DetailCategoryBuilder = InDetailLayoutBuilder.EditCategory(InCategoryName);
    
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 4, 0)
    check(!DetailCategoryBuilder.IsEmpty());
#endif

    TArray<TSharedRef<IPropertyHandle>> PropertyHandles;
    DetailCategoryBuilder.GetDefaultProperties(PropertyHandles);

    for (const TSharedRef<IPropertyHandle>& PropertyHandle : PropertyHandles)
    {
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 3, 0)
        if (PropertyHandle->GetProperty()->GetName() != InPropertyName)
#else
        if (PropertyHandle->GetProperty()->GetName() != InPropertyName.ToString())
#endif
        {
            continue;
        }

        DetailCategoryBuilder.AddProperty(PropertyHandle)
        .CustomWidget()
        .NameContent()
        [
            PropertyHandle->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .MinDesiredWidth(250)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            [
                SNew(SEditableTextBox)
                .Text_Lambda([PropertyHandle]()
                {
                    FString Value;
                    PropertyHandle->GetValue(Value);
                    return FText::FromString(Value);
                })
                .OnTextCommitted_Lambda([PropertyHandle](const FText& InCommittedText, ETextCommit::Type)
                {
                    PropertyHandle->SetValue(InCommittedText.ToString());
                })
                .IsPassword(true)
            ]
        ];
    }
}

#undef LOCTEXT_NAMESPACE
