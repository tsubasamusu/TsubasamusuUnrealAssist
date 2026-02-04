// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SComboButton;
class SWidget;

struct FLocalizedCulturesFlyweight
{
    TArray<FCultureRef> LocalizedCultures;

    FLocalizedCulturesFlyweight()
    {
        constexpr bool bIncludeDerivedCultures = false;

        const TArray<FString> LocalizedCultureNames = FTextLocalizationManager::Get().GetLocalizedCultureNames(ELocalizationLoadFlags::Editor);

        LocalizedCultures = FInternationalization::Get().GetAvailableCultures(LocalizedCultureNames, bIncludeDerivedCultures);
    }
};

DECLARE_DELEGATE_OneParam(FOnLanguageSelected, const FCulturePtr&)

class SLanguageComboButton final : public SCompoundWidget
{
    SLATE_BEGIN_ARGS(SLanguageComboButton) {}
    SLATE_EVENT(FOnLanguageSelected, OnLanguageSelected)
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& InArgs, const TSharedRef<FLocalizedCulturesFlyweight>& InLocalizedCulturesFlyweight);

private:
    static FText GetDesiredComboButtonText();
    TSharedRef<SWidget> OnGetComboButtonMenuContent();
    void OnSelectionChanged(FCulturePtr InSelectedCulture, ESelectInfo::Type /*SelectInfo*/) const;
    bool IsCulturePickable(FCulturePtr InCulture) const;
    
    FOnLanguageSelected OnLanguageSelected;
    TSharedPtr<FLocalizedCulturesFlyweight> LocalizedCulturesFlyweight;
    TSharedPtr<SComboButton> ComboButton;
};
