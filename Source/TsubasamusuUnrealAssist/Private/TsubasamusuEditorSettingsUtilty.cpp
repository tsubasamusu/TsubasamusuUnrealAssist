// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "TsubasamusuEditorSettingsUtilty.h"
#include "GraphEditorSettings.h"
#include "Internationalization/Culture.h"
#include "InternationalizationSettingsModel.h"
#include "Settings/EditorStyleSettings.h"

void FTsubasamusuEditorSettingsUtilty::SetupEditorSettingsForTsubasamusu()
{
    SetupLoadingSavingSettings();
    SetupStyleSettings();
    SetupGraphSettings();
    SetupInternationalizationSettings();
}

void FTsubasamusuEditorSettingsUtilty::SetupLoadingSavingSettings()
{
    UEditorLoadingSavingSettings* EditorLoadingSavingSettings = GetMutableDefault<UEditorLoadingSavingSettings>();

    EditorLoadingSavingSettings->bAutoSaveEnable = false;
}

void FTsubasamusuEditorSettingsUtilty::SetupStyleSettings()
{
    UEditorStyleSettings* EditorStyleSettings = GetMutableDefault<UEditorStyleSettings>();

    EditorStyleSettings->AssetEditorOpenLocation = EAssetEditorOpenLocation::MainWindow;
}

void FTsubasamusuEditorSettingsUtilty::SetupGraphSettings()
{
    UGraphEditorSettings* GraphEditorSettings = GetMutableDefault<UGraphEditorSettings>();

    GraphEditorSettings->bShowCommentBubbleWhenZoomedOut = true;
    GraphEditorSettings->DefaultCommentNodeTitleColor = FLinearColor::Black;
}

void FTsubasamusuEditorSettingsUtilty::SetupInternationalizationSettings()
{
    UInternationalizationSettingsModel* InternationalizationSettingsModel = GetMutableDefault<UInternationalizationSettingsModel>();

    FInternationalization::Get().SetCurrentLanguageAndLocale(TEXT("en"));

    InternationalizationSettingsModel->SetShouldUseLocalizedNodeAndPinNames(false);
    InternationalizationSettingsModel->SetShouldUseLocalizedNumericInput(false);
    InternationalizationSettingsModel->SetShouldUseLocalizedPropertyNames(false);
}