// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "Setting/TsubasamusuEditorSettingsUtility.h"
#include "GraphEditorSettings.h"
#include "InternationalizationSettingsModel.h"
#include "Settings/EditorStyleSettings.h"
#include "BlueprintEditorSettings.h"
#include "OutputLogSettings.h"

void FTsubasamusuEditorSettingsUtility::ChangeEditorSettingsForTsubasamusu()
{
    ChangeLoadingSavingSettingsForTsubasamusu();
    ChangeStyleSettingsForTsubasamusu();
    ChangeGraphEditorSettingsForTsubasamusu();
    ChangeInternationalizationSettingsForTsubasamusu();
    ChangeBlueprintEditorSettingsForTsubasamusu();
    ChangeOutputLogSettingsForTsubasamusu();
    ChangeLevelEditorPlaySettingsForTsubasamusu();
}

void FTsubasamusuEditorSettingsUtility::ChangeLoadingSavingSettingsForTsubasamusu()
{
    UEditorLoadingSavingSettings* EditorLoadingSavingSettings = GetSettingsChecked<UEditorLoadingSavingSettings>();

    EditorLoadingSavingSettings->bAutoSaveEnable = false;

#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3)
    EditorLoadingSavingSettings->RestoreOpenAssetTabsOnRestart = ERestoreOpenAssetTabsMethod::NeverRestore;
#else
    EditorLoadingSavingSettings->bRestoreOpenAssetTabsOnRestart = false;
#endif

    EditorLoadingSavingSettings->SaveConfig();
    EditorLoadingSavingSettings->PostEditChange();
}

void FTsubasamusuEditorSettingsUtility::ChangeStyleSettingsForTsubasamusu()
{
    UEditorStyleSettings* EditorStyleSettings = GetSettingsChecked<UEditorStyleSettings>();

    EditorStyleSettings->AssetEditorOpenLocation = EAssetEditorOpenLocation::MainWindow;
    EditorStyleSettings->bUseSmallToolBarIcons = true;

    EditorStyleSettings->SaveConfig();
    EditorStyleSettings->PostEditChange();
}

void FTsubasamusuEditorSettingsUtility::ChangeGraphEditorSettingsForTsubasamusu()
{
    UGraphEditorSettings* GraphEditorSettings = GetSettingsChecked<UGraphEditorSettings>();

    GraphEditorSettings->bShowCommentBubbleWhenZoomedOut = true;
    GraphEditorSettings->DefaultCommentNodeTitleColor = FLinearColor::Black;

    GraphEditorSettings->SaveConfig();
    GraphEditorSettings->PostEditChange();
}

void FTsubasamusuEditorSettingsUtility::ChangeInternationalizationSettingsForTsubasamusu()
{
    UInternationalizationSettingsModel* InternationalizationSettingsModel = GetSettingsChecked<UInternationalizationSettingsModel>();

    FInternationalization::Get().SetCurrentLanguageAndLocale(TEXT("en"));

    InternationalizationSettingsModel->SetShouldUseLocalizedNodeAndPinNames(false);
    InternationalizationSettingsModel->SetShouldUseLocalizedNumericInput(false);
    InternationalizationSettingsModel->SetShouldUseLocalizedPropertyNames(false);

    InternationalizationSettingsModel->SaveConfig();
    InternationalizationSettingsModel->PostEditChange();
}

void FTsubasamusuEditorSettingsUtility::ChangeBlueprintEditorSettingsForTsubasamusu()
{
    UBlueprintEditorSettings* BlueprintEditorSettings = GetSettingsChecked<UBlueprintEditorSettings>();

    BlueprintEditorSettings->bSpawnDefaultBlueprintNodes = false;
    BlueprintEditorSettings->bFavorPureCastNodes = true;

    BlueprintEditorSettings->SaveConfig();
    BlueprintEditorSettings->PostEditChange();
}

void FTsubasamusuEditorSettingsUtility::ChangeOutputLogSettingsForTsubasamusu()
{
    UOutputLogSettings* OutputLogSettings = GetSettingsChecked<UOutputLogSettings>();

    OutputLogSettings->CategoryColorizationMode = ELogCategoryColorizationMode::ColorizeWholeLine;
    
    OutputLogSettings->SaveConfig();
    OutputLogSettings->PostEditChange();
}

void FTsubasamusuEditorSettingsUtility::ChangeLevelEditorPlaySettingsForTsubasamusu()
{
    ULevelEditorPlaySettings* LevelEditorPlaySettings = GetSettingsChecked<ULevelEditorPlaySettings>();
    
    LevelEditorPlaySettings->GameGetsMouseControl = true;
    
    LevelEditorPlaySettings->SaveConfig();
    LevelEditorPlaySettings->PostEditChange();
}
