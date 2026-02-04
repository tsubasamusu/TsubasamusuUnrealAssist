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
			MakeCommentGenerationLanguageSameAsEditorLanguage();
		}
	}
}

FCulturePtr UTsubasamusuUnrealAssistSettings::GetCommentGenerationLanguageCulture() const
{
	const FCulturePtr CommentGenerationLanguageCulture = FInternationalization::Get().GetCulture(LanguageCultureNameForCommentGeneration);
	checkf(CommentGenerationLanguageCulture.IsValid(), TEXT("The comment generation language culture name \"%s\" is invalid."), *LanguageCultureNameForCommentGeneration);
	return CommentGenerationLanguageCulture;
}

void UTsubasamusuUnrealAssistSettings::SetCommentGenerationLanguageCulture(const FCulturePtr& NewCommentGenerationLanguageCulture)
{
	LanguageCultureNameForCommentGeneration = NewCommentGenerationLanguageCulture->GetName();
	SaveConfig();
}

void UTsubasamusuUnrealAssistSettings::MakeCommentGenerationLanguageSameAsEditorLanguage()
{
	const FCultureRef EditorLanguageCulture = GetEditorLanguageCulture();
	SetCommentGenerationLanguageCulture(EditorLanguageCulture);
}

FCultureRef UTsubasamusuUnrealAssistSettings::GetEditorLanguageCulture()
{
	return FInternationalization::Get().GetCurrentLanguage();
}
