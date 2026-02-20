// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "Setting/RecommendedEditorSettingsApplier.h"
#include "GraphEditorSettings.h"
#include "InternationalizationSettingsModel.h"
#include "BlueprintEditorSettings.h"
#include "EditorSettingsUtility.h"

#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1)
#include "Settings/EditorStyleSettings.h"
#include "OutputLogSettings.h"
#else
#include "Classes/EditorStyleSettings.h"
#endif

void FRecommendedEditorSettingsApplier::ApplyRecommendedEditorSettings()
{
    ApplyRecommendedLoadingSavingSettings();
    ApplyRecommendedStyleSettings();
    ApplyRecommendedGraphEditorSettings();
    ApplyRecommendedInternationalizationSettings();
    ApplyRecommendedBlueprintEditorSettings();
    ApplyRecommendedLevelEditorPlaySettings();
    
#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1)
    ApplyRecommendedOutputLogSettings();
#endif
}

void FRecommendedEditorSettingsApplier::ApplyRecommendedLoadingSavingSettings()
{
    UEditorLoadingSavingSettings* EditorLoadingSavingSettings = FEditorSettingsUtility::GetSettingsChecked<UEditorLoadingSavingSettings>();

    EditorLoadingSavingSettings->bAutoSaveEnable = false;

#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3)
    EditorLoadingSavingSettings->RestoreOpenAssetTabsOnRestart = ERestoreOpenAssetTabsMethod::NeverRestore;
#else
    EditorLoadingSavingSettings->bRestoreOpenAssetTabsOnRestart = false;
#endif

    EditorLoadingSavingSettings->SaveConfig();
    EditorLoadingSavingSettings->PostEditChange();
}

void FRecommendedEditorSettingsApplier::ApplyRecommendedStyleSettings()
{
    UEditorStyleSettings* EditorStyleSettings = FEditorSettingsUtility::GetSettingsChecked<UEditorStyleSettings>();

    EditorStyleSettings->AssetEditorOpenLocation = EAssetEditorOpenLocation::MainWindow;
    EditorStyleSettings->bUseSmallToolBarIcons = true;

#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 0)
    EditorStyleSettings->CategoryColorizationMode = ELogCategoryColorizationMode::ColorizeWholeLine;
#endif
    
    EditorStyleSettings->SaveConfig();
    EditorStyleSettings->PostEditChange();
}

void FRecommendedEditorSettingsApplier::ApplyRecommendedGraphEditorSettings()
{
    UGraphEditorSettings* GraphEditorSettings = FEditorSettingsUtility::GetSettingsChecked<UGraphEditorSettings>();

    GraphEditorSettings->bShowCommentBubbleWhenZoomedOut = true;
    GraphEditorSettings->DefaultCommentNodeTitleColor = FLinearColor::Black;

    GraphEditorSettings->SaveConfig();
    GraphEditorSettings->PostEditChange();
}

void FRecommendedEditorSettingsApplier::ApplyRecommendedInternationalizationSettings()
{
    UInternationalizationSettingsModel* InternationalizationSettingsModel = FEditorSettingsUtility::GetSettingsChecked<UInternationalizationSettingsModel>();

    // Set the editor language to English.
    {
        const FString EnglishCultureName = TEXT("en");
        InternationalizationSettingsModel->SetEditorLanguage(EnglishCultureName);
        FInternationalization::Get().SetCurrentLanguage(EnglishCultureName);

        for (TObjectIterator<UClass> ClassIterator; ClassIterator; ++ClassIterator)
        {
            const UClass* CurrentClass = *ClassIterator;
            const UEdGraphSchema* Schema = Cast<UEdGraphSchema>(CurrentClass->GetDefaultObject());

            if (IsValid(Schema))
            {
                Schema->ForceVisualizationCacheClear();
            }
        }
    }
    
    InternationalizationSettingsModel->SetShouldUseLocalizedNodeAndPinNames(false);
    InternationalizationSettingsModel->SetShouldUseLocalizedNumericInput(false);
    InternationalizationSettingsModel->SetShouldUseLocalizedPropertyNames(false);

    InternationalizationSettingsModel->SaveConfig();
    InternationalizationSettingsModel->PostEditChange();
}

void FRecommendedEditorSettingsApplier::ApplyRecommendedBlueprintEditorSettings()
{
    UBlueprintEditorSettings* BlueprintEditorSettings = FEditorSettingsUtility::GetSettingsChecked<UBlueprintEditorSettings>();

    BlueprintEditorSettings->bSpawnDefaultBlueprintNodes = false;
    BlueprintEditorSettings->bFavorPureCastNodes = true;

    BlueprintEditorSettings->SaveConfig();
    BlueprintEditorSettings->PostEditChange();
}

void FRecommendedEditorSettingsApplier::ApplyRecommendedLevelEditorPlaySettings()
{
    ULevelEditorPlaySettings* LevelEditorPlaySettings = FEditorSettingsUtility::GetSettingsChecked<ULevelEditorPlaySettings>();
    
    LevelEditorPlaySettings->GameGetsMouseControl = true;
    
    LevelEditorPlaySettings->SaveConfig();
    LevelEditorPlaySettings->PostEditChange();
}

#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1)
void FRecommendedEditorSettingsApplier::ApplyRecommendedOutputLogSettings()
{
    UOutputLogSettings* OutputLogSettings = FEditorSettingsUtility::GetSettingsChecked<UOutputLogSettings>();
    
    OutputLogSettings->CategoryColorizationMode = ELogCategoryColorizationMode::ColorizeWholeLine;
    
    OutputLogSettings->SaveConfig();
    OutputLogSettings->PostEditChange();
}
#endif
