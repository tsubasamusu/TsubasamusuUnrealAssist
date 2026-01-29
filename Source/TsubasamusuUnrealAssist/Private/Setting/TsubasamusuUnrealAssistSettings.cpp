// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "Setting/TsubasamusuUnrealAssistSettings.h"
#include "Internationalization/Culture.h"

UTsubasamusuUnrealAssistSettings::UTsubasamusuUnrealAssistSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Comment Translation
	DeeplApiKey = TEXT("");

	// Comment Generation
	OpenAiApiKey = TEXT("");
	GptModelName = TEXT("gpt-4o-mini");
	GptLanguageCultureName = FInternationalization::Get().GetCurrentLanguage()->GetName();
	bIgnoreNodesDoNotHaveConnectedPins = true;
	bIgnoreCommentNodes = false;
	CommentGenerationConditions = { TEXT("answer briefly") };
}

FCulturePtr UTsubasamusuUnrealAssistSettings::GetGptLanguageCulture() const
{
	return FInternationalization::Get().GetCulture(GptLanguageCultureName);
}

void UTsubasamusuUnrealAssistSettings::SetGptLanguageCulture(const FCulturePtr& InGptLanguageCulture)
{
	GptLanguageCultureName = InGptLanguageCulture->GetName();
	SaveConfig();
}

UTsubasamusuUnrealAssistSettings* UTsubasamusuUnrealAssistSettings::GetSettingsChecked()
{
	UTsubasamusuUnrealAssistSettings* Settings = GetMutableDefault<UTsubasamusuUnrealAssistSettings>();
	check(Settings);
	return Settings;
}
