// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "TsubasamusuUnrealAssistModule.h"
#include "TsubasamusuNodeFactory.h"
#include "TsubasamusuEditorSettingsUtility.h"

#define LOCTEXT_NAMESPACE "FTsubasamusuUnrealAssistModule"
#define TUA_IS_ENABLED (!IS_MONOLITHIC && !UE_BUILD_SHIPPING && !UE_BUILD_TEST && !UE_GAME && !UE_SERVER)

void FTsubasamusuUnrealAssistModule::StartupModule()
{
#if TUA_IS_ENABLED
	//FTsubasamusuEditorSettingsUtility::SetupEditorSettingsForTsubasamusu();

	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FTsubasamusuUnrealAssistModule::RegisterTsubasamusuNodeFactory);
#endif
}

void FTsubasamusuUnrealAssistModule::ShutdownModule()
{
#if TUA_IS_ENABLED
	UnregisterTsubasamusuNodeFactory();
#endif
}

void FTsubasamusuUnrealAssistModule::RegisterTsubasamusuNodeFactory()
{
	TsubasamusuNodeFactory = MakeShared<FTsubasamusuNodeFactory>();

	FEdGraphUtilities::RegisterVisualNodeFactory(TsubasamusuNodeFactory);
}

void FTsubasamusuUnrealAssistModule::UnregisterTsubasamusuNodeFactory()
{
	FEdGraphUtilities::UnregisterVisualNodeFactory(TsubasamusuNodeFactory);

	TsubasamusuNodeFactory.Reset();
}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FTsubasamusuUnrealAssistModule, TsubasamusuUnrealAssist)
