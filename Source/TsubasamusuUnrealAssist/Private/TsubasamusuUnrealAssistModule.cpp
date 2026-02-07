// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "TsubasamusuUnrealAssistModule.h"
#include "ISettingsModule.h"
#include "ITickEventHandler.h"
#include "NodeUtility/SelectedNodeMenuExtender.h"
#include "Setting/TsubasamusuSettingsCustomization.h"
#include "Setting/TsubasamusuUnrealAssistSettings.h"
#include "Internationalization/Internationalization.h"
#include "Setting/TsubasamusuEditorSettingsUtility.h"

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
	UnregisterSettingsCustomization();
	UnregisterSettings();
	UnregisterTicker();
}

void FTsubasamusuUnrealAssistModule::ReregisterTicker()
{
	UnregisterTicker();
	RegisterTicker();
}

void FTsubasamusuUnrealAssistModule::RegisterOnPostEngineInitEvent()
{
	OnPostEngineInitHandle = FCoreDelegates::OnPostEngineInit.AddLambda([this]()
	{
		FSelectedNodeMenuExtender::RegisterSelectedNodeMenu();
		RegisterOnEditorLanguageChangedEvent();
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
		UTsubasamusuUnrealAssistSettings* Settings = GetMutableDefault<UTsubasamusuUnrealAssistSettings>();
		
		if (Settings->bUseEditorLanguageForCommentGeneration)
		{
			Settings->MakeCommentGenerationLanguageSameAsEditorLanguage();
		}
	});
}

void FTsubasamusuUnrealAssistModule::UnregisterOnEditorLanguageChangedEvent()
{
	FInternationalization::Get().OnCultureChanged().Remove(OnEditorLanguageChangedHandle);
}

void FTsubasamusuUnrealAssistModule::RegisterTicker()
{
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FTsubasamusuEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
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

bool FTsubasamusuUnrealAssistModule::Tick(const float DeltaTime)
{
	return true;
}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FTsubasamusuUnrealAssistModule, TsubasamusuUnrealAssist)
