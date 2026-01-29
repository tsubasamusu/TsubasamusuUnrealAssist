// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "Setting/SLanguageComboButton.h"
#include "Internationalization/Culture.h"
#include "SCulturePicker.h"
#include "Setting/TsubasamusuUnrealAssistSettings.h"
#include "Widgets/Input/SComboButton.h"

#define LOCTEXT_NAMESPACE "LanguageComboButton"

void SLanguageComboButton::Construct(const FArguments& InArgs, const TSharedRef<FLocalizedCulturesFlyweight>& InLocalizedCulturesFlyweight)
{
    LocalizedCulturesFlyweight = InLocalizedCulturesFlyweight;

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
    const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();
    const FString GptLanguageName = TsubasamusuUnrealAssistSettings->GetGptLanguageCulture()->GetNativeName();
    return FText::FromString(GptLanguageName);
}

TSharedRef<SWidget> SLanguageComboButton::OnGetComboButtonMenuContent()
{
    const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();

    const TSharedRef<SCulturePicker> CulturePicker = SNew(SCulturePicker)
        .InitialSelection(TsubasamusuUnrealAssistSettings->GetGptLanguageCulture())
        .OnSelectionChanged(this, &SLanguageComboButton::OnSelectionChanged)
        .IsCulturePickable(this, &SLanguageComboButton::IsCulturePickable)
        .DisplayNameFormat(SCulturePicker::ECultureDisplayFormat::ActiveAndNativeCultureDisplayName)
        .ViewMode(SCulturePicker::ECulturesViewMode::Flat);

    TSharedRef<SBox> Content = SNew(SBox)
        .MaxDesiredHeight(500.0f)
        .WidthOverride(350.0f)
        [
            CulturePicker
        ];
    
    return Content;
}

void SLanguageComboButton::OnSelectionChanged(FCulturePtr InSelectedCulture, ESelectInfo::Type /*SelectInfo*/) const
{
    UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = UTsubasamusuUnrealAssistSettings::GetSettingsChecked();
    TsubasamusuUnrealAssistSettings->SetGptLanguageCulture(InSelectedCulture);

    if (ComboButton.IsValid())
    {
        ComboButton->SetIsOpen(false);
    }
}

bool SLanguageComboButton::IsCulturePickable(FCulturePtr InCulture) const
{
    return LocalizedCulturesFlyweight->LocalizedCultures.Contains(InCulture);
}

#undef LOCTEXT_NAMESPACE
