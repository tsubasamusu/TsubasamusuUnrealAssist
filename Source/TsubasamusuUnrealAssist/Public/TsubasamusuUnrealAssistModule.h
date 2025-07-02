// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class IAssetTypeActions;
class ISettingsModule;
class FTsubasamusuNodeFactory;

class FTsubasamusuUnrealAssistModule final : public IModuleInterface
{
public:
	
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

	void RegisterOnPostEngineInitEvent();
	void UnregisterOnPostEngineInitEvent() const;
	
	void RegisterTsubasamusuNodeFactory();
	void UnregisterTsubasamusuNodeFactory();

	void RegisterSettings() const;
	void UnregisterSettings() const;

	void RegisterAssetTypeActions();
	void UnregisterAssetTypeActions();
	
	static ISettingsModule* GetSettingsModuleChecked();
	
	TSharedPtr<FTsubasamusuNodeFactory> TsubasamusuNodeFactoryPtr;
	TArray<TSharedPtr<IAssetTypeActions>> CreatedAssetTypeActions;
	FDelegateHandle OnPostEngineInitHandle;
	
	const FName SettingsContainerName = TEXT("Editor");
	const FName SettingsCategoryName = TEXT("Plugins");
	const FName SettingsSectionName = TEXT("Tsubasamusu Unreal Assist");
};
