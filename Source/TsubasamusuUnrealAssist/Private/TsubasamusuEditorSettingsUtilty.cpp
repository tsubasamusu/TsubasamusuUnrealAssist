// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "TsubasamusuEditorSettingsUtilty.h"
#include "GraphEditorSettings.h"
#include "Internationalization/Culture.h"
#include "InternationalizationSettingsModel.h"
#include "Settings/EditorStyleSettings.h"
#include "BlueprintEditorSettings.h"

void FTsubasamusuEditorSettingsUtilty::SetupEditorSettingsForTsubasamusu()
{
    SetupLoadingSavingSettings();
    SetupStyleSettings();
    SetupGraphEditorSettings();
    SetupInternationalizationSettings();
    SetupBlueprintEditorSettings();
}

void FTsubasamusuEditorSettingsUtilty::SetupLoadingSavingSettings()
{
    UEditorLoadingSavingSettings* EditorLoadingSavingSettings = GetSettingsChecked<UEditorLoadingSavingSettings>();

    EditorLoadingSavingSettings->bAutoSaveEnable = false;
}

void FTsubasamusuEditorSettingsUtilty::SetupStyleSettings()
{
    UEditorStyleSettings* EditorStyleSettings = GetSettingsChecked<UEditorStyleSettings>();

    EditorStyleSettings->AssetEditorOpenLocation = EAssetEditorOpenLocation::MainWindow;
}

void FTsubasamusuEditorSettingsUtilty::SetupGraphEditorSettings()
{
    UGraphEditorSettings* GraphEditorSettings = GetSettingsChecked<UGraphEditorSettings>();

    GraphEditorSettings->bShowCommentBubbleWhenZoomedOut = true;
    GraphEditorSettings->DefaultCommentNodeTitleColor = FLinearColor::Black;
}

void FTsubasamusuEditorSettingsUtilty::SetupInternationalizationSettings()
{
    UInternationalizationSettingsModel* InternationalizationSettingsModel = GetSettingsChecked<UInternationalizationSettingsModel>();

    FInternationalization::Get().SetCurrentLanguageAndLocale(TEXT("en"));

    InternationalizationSettingsModel->SetShouldUseLocalizedNodeAndPinNames(false);
    InternationalizationSettingsModel->SetShouldUseLocalizedNumericInput(false);
    InternationalizationSettingsModel->SetShouldUseLocalizedPropertyNames(false);
}

void FTsubasamusuEditorSettingsUtilty::SetupBlueprintEditorSettings()
{
    UBlueprintEditorSettings* BlueprintEditorSettings = GetSettingsChecked<UBlueprintEditorSettings>();

    BlueprintEditorSettings->bSpawnDefaultBlueprintNodes = false;
}