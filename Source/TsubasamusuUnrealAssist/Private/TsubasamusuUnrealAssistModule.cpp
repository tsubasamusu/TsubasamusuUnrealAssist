// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "TsubasamusuUnrealAssistModule.h"
#include "AssetToolsModule.h"
#include "AssetTypeActions_TsubasamusuBlueprint.h"
#include "IAssetTools.h"
#include "ISettingsModule.h"
#include "NodeUtility/SelectedNodeMenuExtender.h"
#include "Setting/TsubasamusuSettingsCustomization.h"
#include "Setting/TsubasamusuUnrealAssistSettings.h"
#include "Internationalization/Internationalization.h"
#include "NodeUtility/NodePreviewer.h"
#include "Setting/TsubasamusuEditorSettingsUtility.h"

#define LOCTEXT_NAMESPACE "TsubasamusuUnrealAssist"

void FTsubasamusuUnrealAssistModule::StartupModule()
{
	RegisterSettings();
	RegisterSettingsCustomization();
	RegisterOnPostEngineInitEvent();
	RegisterTicker();
	RegisterAssetTypeActions();
}

void FTsubasamusuUnrealAssistModule::ShutdownModule()
{
	UnregisterOnPostEngineInitEvent();
	UnregisterOnEditorLanguageChangedEvent();
	UnregisterSettingsCustomization();
	UnregisterSettings();
	UnregisterTicker();
	UnregisterAssetTypeActions();
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

void FTsubasamusuUnrealAssistModule::RegisterAssetTypeActions()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get();
	const TSharedRef<IAssetTypeActions> AssetTypeActions = MakeShared<FAssetTypeActions_TsubasamusuBlueprint>();
	
	AssetTools.RegisterAssetTypeActions(AssetTypeActions);
	AssetTypeActionsArray.Add(AssetTypeActions);
}

void FTsubasamusuUnrealAssistModule::UnregisterAssetTypeActions()
{
	if (FModuleManager::Get().IsModuleLoaded(TEXT("AssetTools")))
	{
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get();
	
		for (const TSharedPtr<IAssetTypeActions> AssetTypeActions : AssetTypeActionsArray)
		{
			if (AssetTypeActions.IsValid())
			{
				AssetTools.UnregisterAssetTypeActions(AssetTypeActions.ToSharedRef());
			}
		}
	
		AssetTypeActionsArray.Empty();
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
	UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FTsubasamusuEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
	
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
