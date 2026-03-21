// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "TsubasamusuUnrealAssistModule.h"
#include "ISettingsModule.h"
#include "Blueprint/AccessSpecifierOptimizer.h"
#include "Setting/TsubasamusuSettingsCustomization.h"
#include "Setting/TsubasamusuUnrealAssistSettings.h"
#include "Internationalization/Internationalization.h"
#include "Blueprint/UnusedFunctionDeleter.h"
#include "Blueprint/UnusedLocalVariableDeleter.h"
#include "Command/TsubasamusuBlueprintEditorCommands.h"

#define LOCTEXT_NAMESPACE "FTsubasamusuUnrealAssistModule"

void FTsubasamusuUnrealAssistModule::StartupModule()
{
	RegisterSettings();
	RegisterSettingsCustomization();
	
	PostEngineInitHandle = FCoreDelegates::OnPostEngineInit.AddRaw(this, &FTsubasamusuUnrealAssistModule::RegisterAssetEditorOpenedEvent);
}

void FTsubasamusuUnrealAssistModule::ShutdownModule()
{
	UnregisterSettings();
	UnregisterSettingsCustomization();
	UnregisterAssetEditorOpenedEvent();
	
	if (PostEngineInitHandle.IsValid())
	{
		FCoreDelegates::OnPostEngineInit.Remove(PostEngineInitHandle);
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

void FTsubasamusuUnrealAssistModule::RegisterAssetEditorOpenedEvent()
{
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	
	AssetEditorOpenedHandle = AssetEditorSubsystem->OnAssetEditorOpened().AddLambda([this](UObject* InOpenedAsset)
	{
		UBlueprint* OpenedBlueprint = Cast<UBlueprint>(InOpenedAsset);
	
		if (IsValid(OpenedBlueprint))
		{
			FTsubasamusuBlueprintEditorCommands::Register();

			FAccessSpecifierOptimizer::RegisterOptimizeAccessSpecifiersMenu(OpenedBlueprint);
			FUnusedFunctionDeleter::RegisterDeleteUnusedFunctionsMenu(OpenedBlueprint);
			FUnusedLocalVariableDeleter::RegisterDeleteUnusedLocalVariablesMenu(OpenedBlueprint);
		}
	});
}

void FTsubasamusuUnrealAssistModule::UnregisterAssetEditorOpenedEvent() const
{
	if (AssetEditorOpenedHandle.IsValid() && IsValid(GEditor))
	{
		UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
		AssetEditorSubsystem->OnAssetEditorOpened().Remove(AssetEditorOpenedHandle);
	}
}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FTsubasamusuUnrealAssistModule, TsubasamusuUnrealAssist)
