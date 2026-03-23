// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "TickableEditorSubsystem.h"
#include "Setting/EditorSettingsUtility.h"
#include "Setting/TsubasamusuUnrealAssistSettings.h"

void UTickableEditorSubsystem::Deinitialize()
{
	Super::Deinitialize();
	UnregisterTicker();
}

void UTickableEditorSubsystem::OnPostEngineInit()
{
	Super::OnPostEngineInit();
	
	CachedTsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
	
	CachedTsubasamusuUnrealAssistSettings->OnSettingsPropertyChanged.AddLambda([this](const FName& InChangedPropertyName)
	{
		if (InChangedPropertyName == GET_MEMBER_NAME_CHECKED(UTsubasamusuUnrealAssistSettings, TickInterval))
		{
			UnregisterTicker();
			RegisterTicker();
		}
	});
	
	RegisterTicker();
}

void UTickableEditorSubsystem::RegisterTicker()
{
	if (CachedTsubasamusuUnrealAssistSettings.IsValid())
	{
		const FTickerDelegate TickerDelegate = FTickerDelegate::CreateLambda([this](const float InDeltaTime)
		{
			Tick(InDeltaTime);
			return true;
		});
	
		TickerHandle = FTSTicker::GetCoreTicker().AddTicker(TickerDelegate, CachedTsubasamusuUnrealAssistSettings->TickInterval);
	}
}

void UTickableEditorSubsystem::UnregisterTicker() const
{
	if (TickerHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);
	}
}
