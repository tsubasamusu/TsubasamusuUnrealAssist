// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FPlayerCharacterNodeFactory;

class FTsubasamusuUnrealAssistModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterPlayerCharacterNodeFactory();
	void UnregisterPlayerCharacterNodeFactory();

	TSharedPtr<FPlayerCharacterNodeFactory> PlayerCharacterNodeFactory;
};