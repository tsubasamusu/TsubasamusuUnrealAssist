// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "Setting/TsubasamusuEditorSettingsUtility.h"
#include "GraphEditorSettings.h"
#include "InternationalizationSettingsModel.h"
#include "Settings/EditorStyleSettings.h"
#include "BlueprintEditorSettings.h"
#include "OutputLogSettings.h"

void FTsubasamusuEditorSettingsUtility::SetupEditorSettingsForTsubasamusu()
{
    SetupLoadingSavingSettings();
    SetupStyleSettings();
    SetupGraphEditorSettings();
    SetupInternationalizationSettings();
    SetupBlueprintEditorSettings();
    SetupOutputLogSettings();
}

void FTsubasamusuEditorSettingsUtility::SetupLoadingSavingSettings()
{
    UEditorLoadingSavingSettings* EditorLoadingSavingSettings = GetSettingsChecked<UEditorLoadingSavingSettings>();

    EditorLoadingSavingSettings->bAutoSaveEnable = false;
    EditorLoadingSavingSettings->RestoreOpenAssetTabsOnRestart = ERestoreOpenAssetTabsMethod::NeverRestore;

    EditorLoadingSavingSettings->PostEditChange();
}

void FTsubasamusuEditorSettingsUtility::SetupStyleSettings()
{
    UEditorStyleSettings* EditorStyleSettings = GetSettingsChecked<UEditorStyleSettings>();

    EditorStyleSettings->AssetEditorOpenLocation = EAssetEditorOpenLocation::MainWindow;

    EditorStyleSettings->PostEditChange();
}

void FTsubasamusuEditorSettingsUtility::SetupGraphEditorSettings()
{
    UGraphEditorSettings* GraphEditorSettings = GetSettingsChecked<UGraphEditorSettings>();

    GraphEditorSettings->bShowCommentBubbleWhenZoomedOut = true;
    GraphEditorSettings->DefaultCommentNodeTitleColor = FLinearColor::Black;

    GraphEditorSettings->PostEditChange();
}

void FTsubasamusuEditorSettingsUtility::SetupInternationalizationSettings()
{
    UInternationalizationSettingsModel* InternationalizationSettingsModel = GetSettingsChecked<UInternationalizationSettingsModel>();

    FInternationalization::Get().SetCurrentLanguageAndLocale(TEXT("en"));

    InternationalizationSettingsModel->SetShouldUseLocalizedNodeAndPinNames(false);
    InternationalizationSettingsModel->SetShouldUseLocalizedNumericInput(false);
    InternationalizationSettingsModel->SetShouldUseLocalizedPropertyNames(false);

    InternationalizationSettingsModel->PostEditChange();
}

void FTsubasamusuEditorSettingsUtility::SetupBlueprintEditorSettings()
{
    UBlueprintEditorSettings* BlueprintEditorSettings = GetSettingsChecked<UBlueprintEditorSettings>();

    BlueprintEditorSettings->bSpawnDefaultBlueprintNodes = false;
    BlueprintEditorSettings->bFavorPureCastNodes = true;

    BlueprintEditorSettings->PostEditChange();
}

void FTsubasamusuEditorSettingsUtility::SetupOutputLogSettings()
{
    UOutputLogSettings* OutputLogSettings = GetSettingsChecked<UOutputLogSettings>();

    OutputLogSettings->CategoryColorizationMode = ELogCategoryColorizationMode::ColorizeWholeLine;

    OutputLogSettings->PostEditChange();
}
