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
#include "Type/TsubasamusuUnrealAssistMacros.h"

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
    
    ULlmManager* LlmManager = ULlmManager::GetChecked();
    IDetailCategoryBuilder& DetailCategoryBuilder = InDetailLayoutBuilder.EditCategory(CategoryName);

    DetailCategoryBuilder.AddCustomRow(RowFilterText)
    .Visibility(TAttribute<EVisibility>::CreateLambda([LlmManager]()
    {
        return GetRestartLlamaServerMessageVisibility(LlmManager);
    }))
    .WholeRowWidget
    [
        SNew(SBox)
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 1, 0)
        .Padding(0.f, 5.f)
#else
        .Padding(FMargin(0.f, 5.f))
#endif
        [
            SNew(SWarningOrErrorBox)
            .MessageStyle_Lambda([LlmManager]()
            {
                return LlmManager->LlamaServerIsBadStatus() ? EMessageStyle::Error : EMessageStyle::Warning;
            })
            .Message_Lambda([LlmManager]()
            {
                return GetRestartLlamaServerMessageText(LlmManager);
            })
            [
                SNew(SButton)
                .IsEnabled_Lambda([LlmManager]()
                {
                    return LlmManager->LlamaServerCanRestart();
                })
                .Text_Lambda([LlmManager]()
                {
                    const FText StartText = LOCTEXT("StartLlamaServerButtonLabel", "Start Llama Server");
                    const FText RestartText = LOCTEXT("RestartLlamaServerButtonLabel", "Restart Llama Server");
    
                    return LlmManager->LlamaServerIsRunning() ? RestartText : StartText;
                })
                .OnClicked_Lambda([LlmManager]()
                {
                    LlmManager->RestartLlamaServer();
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
    
    const TSharedPtr<IPropertyHandle> UseEditorLanguagePropertyHandle = InDetailLayoutBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UTsubasamusuUnrealAssistSettings, bUseEditorLanguageForCommentGeneration));
    
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

EVisibility FTsubasamusuSettingsCustomization::GetRestartLlamaServerMessageVisibility(const ULlmManager* InLlmManager)
{
    const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
    
    const bool bAppliedCurrentLlamaServerSettings = TsubasamusuUnrealAssistSettings->GetCurrentLlamaServerSettings() == InLlmManager->GetLastAppliedLlamaServerSettings();
    const bool bLlamaServerIsAvailable = InLlmManager->GetLlamaServerStatus() == ELlamaServerStatus::Available;
    
    return bAppliedCurrentLlamaServerSettings && bLlamaServerIsAvailable ? EVisibility::Collapsed : EVisibility::Visible;
}

FText FTsubasamusuSettingsCustomization::GetRestartLlamaServerMessageText(const ULlmManager* InLlmManager)
{
    FText MessageText;
    const ELlamaServerStatus LlamaServerStatus = InLlmManager->GetLlamaServerStatus();
    
    switch (LlamaServerStatus)
    {
    case ELlamaServerStatus::NotStartedYet:
        MessageText = LOCTEXT("RestartLlamaServerMessage_Start", "Llama server has not yet started. You must start the server to use AI-related features.");
        break;
    case ELlamaServerStatus::WhileTryingToStart:
        MessageText = LOCTEXT("RestartLlamaServerMessage_Starting", "Starting server...");
        break;
    case ELlamaServerStatus::LoadingModel:
        MessageText = LOCTEXT("RestartLlamaServerMessage_Loading", "Loading model...");
        break;
    case ELlamaServerStatus::Available:
        MessageText = LOCTEXT("RestartLlamaServerMessage_Restart", "Llama server settings have been updated. Please restart the server to apply these changes.");
        break;
    case ELlamaServerStatus::UnknownInstanceIsRunning:
        MessageText = LOCTEXT("RestartLlamaServerMessage_UnknownInstance", "Unknown Llama server instance is running. Please stop the instance and try starting the server again.");
        break;
    case ELlamaServerStatus::FailedToStart:
        MessageText = LOCTEXT("RestartLlamaServerMessage_FailedToStart", "Llama server failed to start. The server is currently not running. Please check your settings and try starting the server again. For more details, see Output Log.");
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
