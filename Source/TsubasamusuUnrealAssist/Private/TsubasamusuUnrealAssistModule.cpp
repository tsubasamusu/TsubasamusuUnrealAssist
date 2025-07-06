// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "TsubasamusuUnrealAssistModule.h"
#include "AssetToolsModule.h"
#include "BlueprintEditor/AssetTypeActions_TsubasamusuBlueprint.h"
#include "ISettingsModule.h"
#include "BlueprintEditor/Menu/SelectedNodeMenuExtender.h"
#include "BlueprintEditor/TsubasamusuNodeFactory.h"
#include "Setting/TsubasamusuUnrealAssistSettings.h"

#define LOCTEXT_NAMESPACE "FTsubasamusuUnrealAssistModule"
#define TUA_IS_ENABLED (!IS_MONOLITHIC && !UE_BUILD_SHIPPING && !UE_BUILD_TEST && !UE_GAME && !UE_SERVER)

void FTsubasamusuUnrealAssistModule::StartupModule()
{
#if TUA_IS_ENABLED
	RegisterSettings();
	RegisterOnPostEngineInitEvent();
	RegisterAssetTypeActions();
#endif
}

void FTsubasamusuUnrealAssistModule::ShutdownModule()
{
#if TUA_IS_ENABLED
	UnregisterOnPostEngineInitEvent();
	UnregisterTsubasamusuNodeFactory();
	UnregisterAssetTypeActions();
	UnregisterSettings();
#endif
}

void FTsubasamusuUnrealAssistModule::RegisterOnPostEngineInitEvent()
{
	OnPostEngineInitHandle = FCoreDelegates::OnPostEngineInit.AddLambda([this]()
	{
		RegisterTsubasamusuNodeFactory();
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

void FTsubasamusuUnrealAssistModule::RegisterAssetTypeActions()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	const TSharedRef<IAssetTypeActions> AssetTypeActions = MakeShared<FAssetTypeActions_TsubasamusuBlueprint>();
	AssetTools.RegisterAssetTypeActions(AssetTypeActions);
	CreatedAssetTypeActions.Add(AssetTypeActions);
}

void FTsubasamusuUnrealAssistModule::UnregisterAssetTypeActions()
{
	if (!FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return;
	}
	
	IAssetTools& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
	
	for (const TSharedPtr<IAssetTypeActions> CreatedAssetTypeAction : CreatedAssetTypeActions)
	{
		if (CreatedAssetTypeAction.IsValid())
		{
			AssetToolsModule.UnregisterAssetTypeActions(CreatedAssetTypeAction.ToSharedRef());
		}
	}
	
	CreatedAssetTypeActions.Empty();
}

ISettingsModule* FTsubasamusuUnrealAssistModule::GetSettingsModuleChecked()
{
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings"));

	check(SettingsModule);

	return SettingsModule;
}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FTsubasamusuUnrealAssistModule, TsubasamusuUnrealAssist)
