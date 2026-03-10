// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "Setting/TsubasamusuUnrealAssistSettings.h"
#include "TsubasamusuUnrealAssistModule.h"
#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h"

UTsubasamusuUnrealAssistSettings::UTsubasamusuUnrealAssistSettings(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	// General
	TickInterval = 0.f;
	
	// Comment Translator
	DeeplApiKey = TEXT("");

	// Comment Generator
	OpenAiApiKey = TEXT("");
	GptModelName = TEXT("gpt-4o-mini");
	bUseEditorLanguageForCommentGeneration = true;
	LanguageCultureNameForCommentGeneration = GetEditorLanguageCulture()->GetName();
	bIgnoreIsolatedNodesWhenGeneratingComments = true;
	bIgnoreCommentNodesWhenGeneratingComments = false;
	bUseToonFormatForCommentGeneration = true;
	CommentGenerationConditions = { TEXT("answer briefly") };
	
	// Node Previewer
	bEnableNodePreview = false;
	bAlsoPreviewAdvancedView = false;
	NodePreviewScale = 1.f;
	
	// Access Specifier Initializer
	bOverrideVariableDefaultAccessSpecifier = false;
	VariableDefaultAccessSpecifier = ETsubasamusuAccessSpecifier::Public;
	bOverrideFunctionDefaultAccessSpecifier = false;
	FunctionDefaultAccessSpecifier = ETsubasamusuAccessSpecifier::Public;
	bOverrideEventDefaultAccessSpecifier = false;
	EventDefaultAccessSpecifier = ETsubasamusuAccessSpecifier::Public;
}

void UTsubasamusuUnrealAssistSettings::PostEditChangeProperty(FPropertyChangedEvent& InPropertyChangedEvent)
{
	Super::PostEditChangeProperty(InPropertyChangedEvent);

	const FName PropertyName = InPropertyChangedEvent.Property ? InPropertyChangedEvent.Property->GetFName() : NAME_None;
	FTsubasamusuUnrealAssistModule& TsubasamusuUnrealAssist = FModuleManager::LoadModuleChecked<FTsubasamusuUnrealAssistModule>(TEXT("TsubasamusuUnrealAssist"));
	
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UTsubasamusuUnrealAssistSettings, bUseEditorLanguageForCommentGeneration) && bUseEditorLanguageForCommentGeneration)
	{
		MakeCommentGenerationLanguageSameAsEditorLanguage();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UTsubasamusuUnrealAssistSettings, TickInterval))
	{
		TsubasamusuUnrealAssist.ReregisterTicker();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UTsubasamusuUnrealAssistSettings, bEnableNodePreview))
	{
		if (bEnableNodePreview)
		{
			TsubasamusuUnrealAssist.StartNodePreview();
		}
		else
		{
			TsubasamusuUnrealAssist.StopNodePreview();
		}
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UTsubasamusuUnrealAssistSettings, bOverrideVariableDefaultAccessSpecifier) && !bOverrideVariableDefaultAccessSpecifier)
	{
		VariableDefaultAccessSpecifier = ETsubasamusuAccessSpecifier::Public;
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UTsubasamusuUnrealAssistSettings, bOverrideFunctionDefaultAccessSpecifier) && !bOverrideFunctionDefaultAccessSpecifier)
	{
		FunctionDefaultAccessSpecifier = ETsubasamusuAccessSpecifier::Public;
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UTsubasamusuUnrealAssistSettings, bOverrideEventDefaultAccessSpecifier) && !bOverrideEventDefaultAccessSpecifier)
	{
		EventDefaultAccessSpecifier = ETsubasamusuAccessSpecifier::Public;
	}
}

FCulturePtr UTsubasamusuUnrealAssistSettings::GetCommentGenerationLanguageCulture() const
{
	const FCulturePtr CommentGenerationLanguageCulture = FInternationalization::Get().GetCulture(LanguageCultureNameForCommentGeneration);
	checkf(CommentGenerationLanguageCulture.IsValid(), TEXT("The comment generation language culture name \"%s\" is invalid."), *LanguageCultureNameForCommentGeneration);
	return CommentGenerationLanguageCulture;
}

void UTsubasamusuUnrealAssistSettings::SetCommentGenerationLanguageCulture(const FCulturePtr InCommentGenerationLanguageCulture)
{
	LanguageCultureNameForCommentGeneration = InCommentGenerationLanguageCulture->GetName();
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
