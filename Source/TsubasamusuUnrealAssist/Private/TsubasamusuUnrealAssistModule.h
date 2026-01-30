// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class IAssetTypeActions;
class ISettingsModule;

class FTsubasamusuUnrealAssistModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterOnPostEngineInitEvent();
	void UnregisterOnPostEngineInitEvent() const;
	
	void RegisterSettings() const;
	void UnregisterSettings() const;

	static void RegisterSettingsCustomization();
	static void UnregisterSettingsCustomization();
	
	TArray<TSharedPtr<IAssetTypeActions>> CreatedAssetTypeActions;
	FDelegateHandle OnPostEngineInitHandle;
	
	const FName SettingsContainerName = TEXT("Editor");
	const FName SettingsCategoryName = TEXT("Plugins");
	const FName SettingsSectionName = TEXT("Tsubasamusu Unreal Assist");
};
