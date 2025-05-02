// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class ISettingsModule;
class FTsubasamusuNodeFactory;

class FTsubasamusuUnrealAssistModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterTsubasamusuNodeFactory();
	void UnregisterTsubasamusuNodeFactory();

	void RegisterSettings() const;
	void UnregisterSettings() const;

	static ISettingsModule* GetSettingsModuleChecked();

	TSharedPtr<FTsubasamusuNodeFactory> TsubasamusuNodeFactoryPtr;
	
	const FName SettingsContainerName = TEXT("Editor");
	const FName SettingsCategoryName = TEXT("Plugins");
	const FName SettingsSectionName = TEXT("Tsubasamusu Unreal Assist");
};