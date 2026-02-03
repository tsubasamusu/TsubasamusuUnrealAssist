// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "Setting/TsubasamusuUnrealAssistSettings.h"
#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h"

UTsubasamusuUnrealAssistSettings::UTsubasamusuUnrealAssistSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Comment Translation
	DeeplApiKey = TEXT("");

	// Comment Generation
	OpenAiApiKey = TEXT("");
	GptModelName = TEXT("gpt-4o-mini");
	bUseEditorLanguageForCommentGeneration = true;
	LanguageCultureNameForCommentGeneration = GetEditorLanguageCulture()->GetName();
	bIgnoreIsolatedNodesWhenGeneratingComments = true;
	bIgnoreCommentNodesWhenGeneratingComments = false;
	bUseToonFormatForCommentGeneration = true;
	CommentGenerationConditions = { TEXT("answer briefly") };
}

void UTsubasamusuUnrealAssistSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UTsubasamusuUnrealAssistSettings, bUseEditorLanguageForCommentGeneration))
	{
		if (bUseEditorLanguageForCommentGeneration)
		{
			MakeGptLanguageSameAsEditorLanguage();
		}
	}
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

void UTsubasamusuUnrealAssistSettings::MakeGptLanguageSameAsEditorLanguage()
{
	const FCultureRef EditorLanguageCulture = GetEditorLanguageCulture();
	SetGptLanguageCulture(EditorLanguageCulture);
}

FCultureRef UTsubasamusuUnrealAssistSettings::GetEditorLanguageCulture()
{
	return FInternationalization::Get().GetCurrentLanguage();
}
