// Copyright (c) 2026, tsubasamusu All rights reserved.

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
	const FCulturePtr GptLanguageCulture = FInternationalization::Get().GetCulture(GptLanguageCultureName);
	checkf(GptLanguageCulture.IsValid(), TEXT("GPT language culture name \"%s\" is invalid."), *GptLanguageCultureName);
	return GptLanguageCulture;
}

void UTsubasamusuUnrealAssistSettings::SetGptLanguageCulture(const FCulturePtr& NewGptLanguageCulture)
{
	GptLanguageCultureName = NewGptLanguageCulture->GetName();
	SaveConfig();
}

UTsubasamusuUnrealAssistSettings* UTsubasamusuUnrealAssistSettings::GetSettingsChecked()
{
	UTsubasamusuUnrealAssistSettings* Settings = GetMutableDefault<UTsubasamusuUnrealAssistSettings>();
	check(Settings);
	return Settings;
}
