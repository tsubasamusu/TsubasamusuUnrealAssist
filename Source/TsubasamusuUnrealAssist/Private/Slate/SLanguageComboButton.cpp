// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "Slate/SLanguageComboButton.h"
#include "Setting/EditorSettingsUtility.h"
#include "Internationalization/Culture.h"
#include "SCulturePicker.h"
#include "Setting/TsubasamusuUnrealAssistSettings.h"
#include "Type/TsubasamusuUnrealAssistStructs.h"
#include "Widgets/Input/SComboButton.h"

#define LOCTEXT_NAMESPACE "SLanguageComboButton"

void SLanguageComboButton::Construct(const FArguments& InArgs, const TSharedRef<FLocalizedCulturesFlyweight>& InLocalizedCulturesFlyweight)
{
    LocalizedCulturesFlyweight = InLocalizedCulturesFlyweight;
    OnLanguageSelected = InArgs._OnLanguageSelected;

    ChildSlot
    [
        SAssignNew(ComboButton, SComboButton)
        .ButtonContent()
        [
            SNew(STextBlock)
            .Text_Static(&SLanguageComboButton::GetDesiredComboButtonText)
        ]
    ];

    ComboButton->SetOnGetMenuContent(FOnGetContent::CreateSP(this, &SLanguageComboButton::OnGetComboButtonMenuContent));
}

FText SLanguageComboButton::GetDesiredComboButtonText()
{
    const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
    const FString GptLanguageName = TsubasamusuUnrealAssistSettings->GetCommentGenerationLanguageCulture()->GetNativeName();
    return FText::FromString(GptLanguageName);
}

TSharedRef<SWidget> SLanguageComboButton::OnGetComboButtonMenuContent()
{
    const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();

    const TSharedRef<SCulturePicker> CulturePicker = SNew(SCulturePicker)
        .InitialSelection(TsubasamusuUnrealAssistSettings->GetCommentGenerationLanguageCulture())
        .OnSelectionChanged_Lambda([this](FCulturePtr InSelectedCulture, ESelectInfo::Type /*InSelectInfo*/)
        {
            OnLanguageSelected.ExecuteIfBound(InSelectedCulture);

            if (ComboButton.IsValid())
            {
                ComboButton->SetIsOpen(false);
            }
        })
        .IsCulturePickable_Lambda([this](FCulturePtr InCulture)
        {
            return LocalizedCulturesFlyweight->LocalizedCultures.Contains(InCulture);
        })
#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 2)
        .ViewMode(SCulturePicker::ECulturesViewMode::Flat)
#endif
        .DisplayNameFormat(SCulturePicker::ECultureDisplayFormat::ActiveAndNativeCultureDisplayName);
    
    TSharedRef<SBox> Content = SNew(SBox)
        .MaxDesiredHeight(500.0f)
        .WidthOverride(350.0f)
        [
            CulturePicker
        ];
    
    return Content;
}

#undef LOCTEXT_NAMESPACE
