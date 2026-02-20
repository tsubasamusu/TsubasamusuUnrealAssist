// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FNodePreviewer;

class FTsubasamusuUnrealAssistModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	void ReregisterTicker();
	
	void StartNodePreview();
	void StopNodePreview();
	
private:
	void RegisterOnPostEngineInitEvent();
	void UnregisterOnPostEngineInitEvent() const;
	
	void RegisterSettings() const;
	void UnregisterSettings() const;

	void RegisterOnEditorLanguageChangedEvent();
	void UnregisterOnEditorLanguageChangedEvent() const;
	
	void RegisterOnAssetEditorOpenedEvent();
	void UnregisterOnAssetEditorOpenedEvent() const;
	
	void RegisterTicker();
	void UnregisterTicker() const;
	
	static void RegisterSettingsCustomization();
	static void UnregisterSettingsCustomization();
	
	bool Tick(const float /* InDeltaTime */);
	
	FDelegateHandle OnPostEngineInitHandle;
	FDelegateHandle OnEditorLanguageChangedHandle;
	FDelegateHandle OnAssetEditorOpenedHandle;
	FTSTicker::FDelegateHandle TickHandle;
	
	const FName SettingsContainerName = TEXT("Editor");
	const FName SettingsCategoryName = TEXT("Plugins");
	const FName SettingsSectionName = TEXT("Tsubasamusu Unreal Assist");
	
	TSharedPtr<FNodePreviewer> NodePreviewer;
};
