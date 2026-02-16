// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "TsubasamusuUnrealAssistModule.h"
#include "ISettingsModule.h"
#include "OptimizeAccessSpecifiersUtility.h"
#include "NodeUtility/SelectedNodeMenuExtender.h"
#include "Setting/TsubasamusuSettingsCustomization.h"
#include "Setting/TsubasamusuUnrealAssistSettings.h"
#include "Internationalization/Internationalization.h"
#include "NodeUtility/NodePreviewer.h"
#include "Setting/EditorSettingsUtility.h"

#define LOCTEXT_NAMESPACE "TsubasamusuUnrealAssist"

void FTsubasamusuUnrealAssistModule::StartupModule()
{
	RegisterSettings();
	RegisterSettingsCustomization();
	RegisterOnPostEngineInitEvent();
	RegisterTicker();
}

void FTsubasamusuUnrealAssistModule::ShutdownModule()
{
	UnregisterOnPostEngineInitEvent();
	UnregisterOnEditorLanguageChangedEvent();
	UnregisterOnAssetEditorOpenedEvent();
	UnregisterSettingsCustomization();
	UnregisterSettings();
	UnregisterTicker();
}

void FTsubasamusuUnrealAssistModule::ReregisterTicker()
{
	UnregisterTicker();
	RegisterTicker();
}

void FTsubasamusuUnrealAssistModule::StartNodePreview()
{
	if (!NodePreviewer.IsValid())
	{
		NodePreviewer = MakeShared<FNodePreviewer>();
	}
}

void FTsubasamusuUnrealAssistModule::StopNodePreview()
{
	if (NodePreviewer.IsValid())
	{
		NodePreviewer.Reset();
	}
}

void FTsubasamusuUnrealAssistModule::RegisterOnPostEngineInitEvent()
{
	OnPostEngineInitHandle = FCoreDelegates::OnPostEngineInit.AddLambda([this]()
	{
		FSelectedNodeMenuExtender::RegisterSelectedNodeMenu();
		RegisterOnEditorLanguageChangedEvent();
		RegisterOnAssetEditorOpenedEvent();
	});
}

void FTsubasamusuUnrealAssistModule::UnregisterOnPostEngineInitEvent() const
{
	if (OnPostEngineInitHandle.IsValid())
	{
		FCoreDelegates::OnPostEngineInit.Remove(OnPostEngineInitHandle);
	}
}

void FTsubasamusuUnrealAssistModule::RegisterSettings() const
{
	const FText SettingsDisplayName = LOCTEXT("TsubasamusuUnrealAssistSettingsDisplayName", "Tsubasamusu Unreal Assist");
	const FText SettingsDescription = LOCTEXT("TsubasamusuUnrealAssistSettingsDescription", "Configure the Tsubasamusu Unreal Assist plugin");
	
	ISettingsModule& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>(TEXT("Settings"));
	SettingsModule.RegisterSettings(SettingsContainerName, SettingsCategoryName, SettingsSectionName, SettingsDisplayName, SettingsDescription, GetMutableDefault<UTsubasamusuUnrealAssistSettings>());
}

void FTsubasamusuUnrealAssistModule::UnregisterSettings() const
{
	ISettingsModule& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>(TEXT("Settings"));
	SettingsModule.UnregisterSettings(SettingsContainerName, SettingsCategoryName, SettingsSectionName);
}

void FTsubasamusuUnrealAssistModule::RegisterOnEditorLanguageChangedEvent()
{
	OnEditorLanguageChangedHandle = FInternationalization::Get().OnCultureChanged().AddLambda([]()
	{
		UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
		
		if (TsubasamusuUnrealAssistSettings->bUseEditorLanguageForCommentGeneration)
		{
			TsubasamusuUnrealAssistSettings->MakeCommentGenerationLanguageSameAsEditorLanguage();
		}
	});
}

void FTsubasamusuUnrealAssistModule::UnregisterOnEditorLanguageChangedEvent()
{
	FInternationalization::Get().OnCultureChanged().Remove(OnEditorLanguageChangedHandle);
}

void FTsubasamusuUnrealAssistModule::RegisterOnAssetEditorOpenedEvent()
{
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	check(IsValid(AssetEditorSubsystem));
	
	OnAssetEditorOpenedHandle = AssetEditorSubsystem->OnAssetEditorOpened().AddLambda([](UObject* OpenedAsset)
	{
		UBlueprint* OpenedBlueprint = Cast<UBlueprint>(OpenedAsset);
		
		if (IsValid(OpenedBlueprint))
		{
			FOptimizeAccessSpecifiersUtility::OnBlueprintEditorOpened(OpenedBlueprint);
		}
	});
}

void FTsubasamusuUnrealAssistModule::UnregisterOnAssetEditorOpenedEvent()
{
	if (IsValid(GEditor))
	{
		UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
		check(IsValid(AssetEditorSubsystem));
		
		AssetEditorSubsystem->OnAssetEditorOpened().Remove(OnAssetEditorOpenedHandle);
	}
}

void FTsubasamusuUnrealAssistModule::RegisterTicker()
{
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
	TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FTsubasamusuUnrealAssistModule::Tick), TsubasamusuUnrealAssistSettings->TickInterval);
}

void FTsubasamusuUnrealAssistModule::UnregisterTicker()
{
	if (TickHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
	}
}

void FTsubasamusuUnrealAssistModule::RegisterSettingsCustomization()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
	const FName SettingsClassName = UTsubasamusuUnrealAssistSettings::StaticClass()->GetFName();
	
	PropertyModule.RegisterCustomClassLayout(SettingsClassName, FOnGetDetailCustomizationInstance::CreateStatic(&FTsubasamusuSettingsCustomization::Create));
}

void FTsubasamusuUnrealAssistModule::UnregisterSettingsCustomization()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
	const FName SettingsClassName = UTsubasamusuUnrealAssistSettings::StaticClass()->GetFName();
	
	PropertyModule.UnregisterCustomClassLayout(SettingsClassName);
}

bool FTsubasamusuUnrealAssistModule::Tick(float /* DeltaTime */)
{
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
	
	if (TsubasamusuUnrealAssistSettings->bEnableNodePreview && !NodePreviewer.IsValid())
	{
		StartNodePreview();
	}
	
	if (NodePreviewer.IsValid())
	{
		NodePreviewer->TryPreviewNode();
	}
	
	return true;
}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FTsubasamusuUnrealAssistModule, TsubasamusuUnrealAssist)
