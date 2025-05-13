// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "TsubasamusuUnrealAssistModule.h"
#include "CreateArrayNodeUtility.h"
#include "ISettingsModule.h"
#include "TsubasamusuNodeFactory.h"
#include "TsubasamusuUnrealAssistSettings.h"

#define LOCTEXT_NAMESPACE "FTsubasamusuUnrealAssistModule"
#define TUA_IS_ENABLED (!IS_MONOLITHIC && !UE_BUILD_SHIPPING && !UE_BUILD_TEST && !UE_GAME && !UE_SERVER)

void FTsubasamusuUnrealAssistModule::StartupModule()
{
#if TUA_IS_ENABLED
	RegisterSettings();
	FCreateArrayNodeUtility::RegisterSelectedNodeMenu();
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FTsubasamusuUnrealAssistModule::RegisterTsubasamusuNodeFactory);
#endif
}

void FTsubasamusuUnrealAssistModule::ShutdownModule()
{
#if TUA_IS_ENABLED
	UnregisterTsubasamusuNodeFactory();
	UnregisterSettings();
#endif
}

void FTsubasamusuUnrealAssistModule::RegisterTsubasamusuNodeFactory()
{
	TsubasamusuNodeFactoryPtr = MakeShared<FTsubasamusuNodeFactory>();

	FEdGraphUtilities::RegisterVisualNodeFactory(TsubasamusuNodeFactoryPtr);
}

void FTsubasamusuUnrealAssistModule::UnregisterTsubasamusuNodeFactory()
{
	FEdGraphUtilities::UnregisterVisualNodeFactory(TsubasamusuNodeFactoryPtr);

	TsubasamusuNodeFactoryPtr.Reset();
}

void FTsubasamusuUnrealAssistModule::RegisterSettings() const
{
	const FText SettingsDisplayName = LOCTEXT("SettingsDisplayName", "Tsubasamusu Unreal Assist");
	const FText SettingsDescription = LOCTEXT("SettingsDescription", "Configure the Tsubasamusu Unreal Assist plugin");

	GetSettingsModuleChecked()->RegisterSettings(SettingsContainerName, SettingsCategoryName, SettingsSectionName, SettingsDisplayName, SettingsDescription, GetMutableDefault<UTsubasamusuUnrealAssistSettings>());
}

void FTsubasamusuUnrealAssistModule::UnregisterSettings() const
{
	GetSettingsModuleChecked()->UnregisterSettings(SettingsContainerName, SettingsCategoryName, SettingsSectionName);
}

ISettingsModule* FTsubasamusuUnrealAssistModule::GetSettingsModuleChecked()
{
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings"));

	check(SettingsModule);

	return SettingsModule;
}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FTsubasamusuUnrealAssistModule, TsubasamusuUnrealAssist)
