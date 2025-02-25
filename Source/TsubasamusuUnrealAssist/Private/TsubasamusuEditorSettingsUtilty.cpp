// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "TsubasamusuEditorSettingsUtilty.h"
#include "GraphEditorSettings.h"
#include "Internationalization/Culture.h"
#include "InternationalizationSettingsModel.h"
#include "Settings/EditorStyleSettings.h"
#include "BlueprintEditorSettings.h"
#include "OutputLogSettings.h"

void FTsubasamusuEditorSettingsUtilty::SetupEditorSettingsForTsubasamusu()
{
    SetupLoadingSavingSettings();
    SetupStyleSettings();
    SetupGraphEditorSettings();
    SetupInternationalizationSettings();
    SetupBlueprintEditorSettings();
    SetupOutputLogSettings();
}

void FTsubasamusuEditorSettingsUtilty::SetupLoadingSavingSettings()
{
    UEditorLoadingSavingSettings* EditorLoadingSavingSettings = GetSettingsChecked<UEditorLoadingSavingSettings>();

    EditorLoadingSavingSettings->bAutoSaveEnable = false;
    EditorLoadingSavingSettings->RestoreOpenAssetTabsOnRestart = ERestoreOpenAssetTabsMethod::NeverRestore;

    EditorLoadingSavingSettings->PostEditChange();
}

void FTsubasamusuEditorSettingsUtilty::SetupStyleSettings()
{
    UEditorStyleSettings* EditorStyleSettings = GetSettingsChecked<UEditorStyleSettings>();

    EditorStyleSettings->AssetEditorOpenLocation = EAssetEditorOpenLocation::MainWindow;

    EditorStyleSettings->PostEditChange();
}

void FTsubasamusuEditorSettingsUtilty::SetupGraphEditorSettings()
{
    UGraphEditorSettings* GraphEditorSettings = GetSettingsChecked<UGraphEditorSettings>();

    GraphEditorSettings->bShowCommentBubbleWhenZoomedOut = true;
    GraphEditorSettings->DefaultCommentNodeTitleColor = FLinearColor::Black;

    GraphEditorSettings->PostEditChange();
}

void FTsubasamusuEditorSettingsUtilty::SetupInternationalizationSettings()
{
    UInternationalizationSettingsModel* InternationalizationSettingsModel = GetSettingsChecked<UInternationalizationSettingsModel>();

    FInternationalization::Get().SetCurrentLanguageAndLocale(TEXT("en"));

    InternationalizationSettingsModel->SetShouldUseLocalizedNodeAndPinNames(false);
    InternationalizationSettingsModel->SetShouldUseLocalizedNumericInput(false);
    InternationalizationSettingsModel->SetShouldUseLocalizedPropertyNames(false);

    InternationalizationSettingsModel->PostEditChange();
}

void FTsubasamusuEditorSettingsUtilty::SetupBlueprintEditorSettings()
{
    UBlueprintEditorSettings* BlueprintEditorSettings = GetSettingsChecked<UBlueprintEditorSettings>();

    BlueprintEditorSettings->bSpawnDefaultBlueprintNodes = false;
    BlueprintEditorSettings->bFavorPureCastNodes = true;

    BlueprintEditorSettings->PostEditChange();
}

void FTsubasamusuEditorSettingsUtilty::SetupOutputLogSettings()
{
    UOutputLogSettings* OutputLogSettings = GetSettingsChecked<UOutputLogSettings>();

    OutputLogSettings->CategoryColorizationMode = ELogCategoryColorizationMode::ColorizeWholeLine;

    OutputLogSettings->PostEditChange();
}