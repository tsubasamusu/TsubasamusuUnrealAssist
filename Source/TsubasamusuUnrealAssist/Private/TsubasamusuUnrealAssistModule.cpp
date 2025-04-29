// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "TsubasamusuUnrealAssistModule.h"
#include "PlayerCharacterNodeFactory.h"
#include "TsubasamusuEditorSettingsUtilty.h"

#define LOCTEXT_NAMESPACE "FTsubasamusuUnrealAssistModule"
#define TUA_IS_ENABLED (!IS_MONOLITHIC && !UE_BUILD_SHIPPING && !UE_BUILD_TEST && !UE_GAME && !UE_SERVER)

void FTsubasamusuUnrealAssistModule::StartupModule()
{
#if TUA_IS_ENABLED
	//FTsubasamusuEditorSettingsUtilty::SetupEditorSettingsForTsubasamusu();

	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FTsubasamusuUnrealAssistModule::RegisterPlayerCharacterNodeFactory);
#endif
}

void FTsubasamusuUnrealAssistModule::ShutdownModule()
{
#if TUA_IS_ENABLED
	UnregisterPlayerCharacterNodeFactory();
#endif
}

void FTsubasamusuUnrealAssistModule::RegisterPlayerCharacterNodeFactory()
{
	PlayerCharacterNodeFactory = MakeShared<FPlayerCharacterNodeFactory>();

	FEdGraphUtilities::RegisterVisualNodeFactory(PlayerCharacterNodeFactory);
}

void FTsubasamusuUnrealAssistModule::UnregisterPlayerCharacterNodeFactory()
{
	FEdGraphUtilities::UnregisterVisualNodeFactory(PlayerCharacterNodeFactory);

	PlayerCharacterNodeFactory.Reset();
}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FTsubasamusuUnrealAssistModule, TsubasamusuUnrealAssist)
