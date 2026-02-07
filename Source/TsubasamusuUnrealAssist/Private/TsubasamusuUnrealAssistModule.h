// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FTsubasamusuUnrealAssistModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	void ReregisterTicker();
	
private:
	void RegisterOnPostEngineInitEvent();
	void UnregisterOnPostEngineInitEvent() const;
	
	void RegisterSettings() const;
	void UnregisterSettings() const;

	void RegisterOnEditorLanguageChangedEvent();
	void UnregisterOnEditorLanguageChangedEvent();
	
	void RegisterTicker();
	void UnregisterTicker();
	
	static void RegisterSettingsCustomization();
	static void UnregisterSettingsCustomization();
	
	bool Tick(const float DeltaTime);
	
	FDelegateHandle OnPostEngineInitHandle;
	FDelegateHandle OnEditorLanguageChangedHandle;
	FTSTicker::FDelegateHandle TickHandle;
	
	const FName SettingsContainerName = TEXT("Editor");
	const FName SettingsCategoryName = TEXT("Plugins");
	const FName SettingsSectionName = TEXT("Tsubasamusu Unreal Assist");
};
