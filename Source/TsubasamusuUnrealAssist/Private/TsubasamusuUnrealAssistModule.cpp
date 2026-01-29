// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "TsubasamusuUnrealAssistModule.h"
#include "ISettingsModule.h"
#include "NodeUtility/SelectedNodeMenuExtender.h"
#include "Setting/TsubasamusuSettingsCustomization.h"
#include "Setting/TsubasamusuUnrealAssistSettings.h"

#define LOCTEXT_NAMESPACE "FTsubasamusuUnrealAssistModule"
#define TUA_IS_ENABLED (!IS_MONOLITHIC && !UE_BUILD_SHIPPING && !UE_BUILD_TEST && !UE_GAME && !UE_SERVER)

void FTsubasamusuUnrealAssistModule::StartupModule()
{
#if TUA_IS_ENABLED
	RegisterSettings();
	RegisterSettingsCustomization();
	RegisterOnPostEngineInitEvent();
#endif
}

void FTsubasamusuUnrealAssistModule::ShutdownModule()
{
#if TUA_IS_ENABLED
	UnregisterOnPostEngineInitEvent();
	UnregisterSettingsCustomization();
	UnregisterSettings();
#endif
}

void FTsubasamusuUnrealAssistModule::RegisterOnPostEngineInitEvent()
{
	OnPostEngineInitHandle = FCoreDelegates::OnPostEngineInit.AddLambda([this]()
	{
		FSelectedNodeMenuExtender::RegisterSelectedNodeMenu();
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
	const FText SettingsDisplayName = LOCTEXT("SettingsDisplayName", "Tsubasamusu Unreal Assist");
	const FText SettingsDescription = LOCTEXT("SettingsDescription", "Configure the Tsubasamusu Unreal Assist plugin");
	
	ISettingsModule& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>(TEXT("Settings"));
	SettingsModule.RegisterSettings(SettingsContainerName, SettingsCategoryName, SettingsSectionName, SettingsDisplayName, SettingsDescription, GetMutableDefault<UTsubasamusuUnrealAssistSettings>());
}

void FTsubasamusuUnrealAssistModule::UnregisterSettings() const
{
	ISettingsModule& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>(TEXT("Settings"));
	SettingsModule.UnregisterSettings(SettingsContainerName, SettingsCategoryName, SettingsSectionName);
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

#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FTsubasamusuUnrealAssistModule, TsubasamusuUnrealAssist)
