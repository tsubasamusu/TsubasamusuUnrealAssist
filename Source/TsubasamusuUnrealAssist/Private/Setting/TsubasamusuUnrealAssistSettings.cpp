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
	bUseEditorLanguageForCommentGeneration = true;
	LanguageCultureNameForCommentGeneration = FInternationalization::Get().GetCurrentLanguage()->GetName();
	bIgnoreIsolatedNodesWhenGeneratingComments = true;
	bIgnoreCommentNodesWhenGeneratingComments = false;
	bUseToonFormatForCommentGeneration = true;
	CommentGenerationConditions = { TEXT("answer briefly") };
}

FCulturePtr UTsubasamusuUnrealAssistSettings::GetGptLanguageCulture() const
{
	const FCulturePtr GptLanguageCulture = FInternationalization::Get().GetCulture(LanguageCultureNameForCommentGeneration);
	checkf(GptLanguageCulture.IsValid(), TEXT("GPT language culture name \"%s\" is invalid."), *LanguageCultureNameForCommentGeneration);
	return GptLanguageCulture;
}

void UTsubasamusuUnrealAssistSettings::SetGptLanguageCulture(const FCulturePtr& NewGptLanguageCulture)
{
	LanguageCultureNameForCommentGeneration = NewGptLanguageCulture->GetName();
	SaveConfig();
}

UTsubasamusuUnrealAssistSettings* UTsubasamusuUnrealAssistSettings::GetSettingsChecked()
{
	UTsubasamusuUnrealAssistSettings* Settings = GetMutableDefault<UTsubasamusuUnrealAssistSettings>();
	check(Settings);
	return Settings;
}
