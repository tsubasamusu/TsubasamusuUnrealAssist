// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

class FTsubasamusuUnrealAssistModule final : public IModuleInterface
{
public:
	//~ Begin IModuleInterface Interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End IModuleInterface Interface
	
private:
	void RegisterSettings() const;
	void UnregisterSettings() const;

	static void RegisterSettingsCustomization();
	static void UnregisterSettingsCustomization();
	
	void RegisterAssetEditorOpenedEvent();
	void UnregisterAssetEditorOpenedEvent() const;
	
	FDelegateHandle PostEngineInitHandle;
	FDelegateHandle AssetEditorOpenedHandle;
	
	const FName SettingsContainerName = TEXT("Editor");
	const FName SettingsCategoryName = TEXT("Plugins");
	const FName SettingsSectionName = TEXT("Tsubasamusu Unreal Assist");
};
