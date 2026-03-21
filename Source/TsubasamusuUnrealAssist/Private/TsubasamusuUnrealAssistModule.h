// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FTsubasamusuUnrealAssistModule final : public IModuleInterface
{
public:
	//~ Begin IModuleInterface Interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End IModuleInterface Interface
	
private:
	void RegisterOnPostEngineInitEvent();
	void UnregisterOnPostEngineInitEvent() const;
	
	void RegisterSettings() const;
	void UnregisterSettings() const;

	void RegisterOnEditorLanguageChangedEvent();
	void UnregisterOnEditorLanguageChangedEvent() const;
	
	void RegisterOnAssetEditorOpenedEvent();
	void UnregisterOnAssetEditorOpenedEvent() const;
	
	static void RegisterSettingsCustomization();
	static void UnregisterSettingsCustomization();
	
	FDelegateHandle OnPostEngineInitHandle;
	FDelegateHandle OnEditorLanguageChangedHandle;
	FDelegateHandle OnAssetEditorOpenedHandle;
	
	const FName SettingsContainerName = TEXT("Editor");
	const FName SettingsCategoryName = TEXT("Plugins");
	const FName SettingsSectionName = TEXT("Tsubasamusu Unreal Assist");
};
