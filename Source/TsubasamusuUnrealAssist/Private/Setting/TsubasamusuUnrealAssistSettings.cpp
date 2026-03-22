// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "Setting/TsubasamusuUnrealAssistSettings.h"
#include "Algo/AnyOf.h"
#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h"
#include "LlamaServerOption/LlamaServerOption.h"
#include "Type/TsubasamusuUnrealAssistMacros.h"
#include "Type/TsubasamusuUnrealAssistStructs.h"

UTsubasamusuUnrealAssistSettings::UTsubasamusuUnrealAssistSettings(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	InitializeProperties();
	PostEngineInitHandle = FCoreDelegates::OnPostEngineInit.AddUObject(this, &UTsubasamusuUnrealAssistSettings::RegisterEditorLanguageChangedEvent);
}

UTsubasamusuUnrealAssistSettings::~UTsubasamusuUnrealAssistSettings()
{
	UnregisterEditorLanguageChangedEvent();
	
	if (PostEngineInitHandle.IsValid())
	{
		FCoreDelegates::OnPostEngineInit.Remove(PostEngineInitHandle);
	}
}

void UTsubasamusuUnrealAssistSettings::PostInitProperties()
{
	Super::PostInitProperties();
	RestoreLlamaServerOptions();
}

void UTsubasamusuUnrealAssistSettings::PostEditChangeProperty(FPropertyChangedEvent& InPropertyChangedEvent)
{
	Super::PostEditChangeProperty(InPropertyChangedEvent);
	
	const FName ChangedPropertyName = InPropertyChangedEvent.GetMemberPropertyName();
	
	if (!ChangedPropertyName.IsNone())
	{
		if (OnSettingsPropertyChanged.IsBound())
		{
			OnSettingsPropertyChanged.Broadcast(ChangedPropertyName);
		}
		
		if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UTsubasamusuUnrealAssistSettings, bUseEditorLanguageForCommentGeneration) && bUseEditorLanguageForCommentGeneration)
		{
			MakeCommentGenerationLanguageSameAsEditorLanguage();
		}
		else if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UTsubasamusuUnrealAssistSettings, bOverrideVariableDefaultAccessSpecifier) && !bOverrideVariableDefaultAccessSpecifier)
		{
			VariableDefaultAccessSpecifier = ETsubasamusuAccessSpecifier::Public;
		}
		else if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UTsubasamusuUnrealAssistSettings, bOverrideFunctionDefaultAccessSpecifier) && !bOverrideFunctionDefaultAccessSpecifier)
		{
			FunctionDefaultAccessSpecifier = ETsubasamusuAccessSpecifier::Public;
		}
#if CUSTOM_EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
		else if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UTsubasamusuUnrealAssistSettings, bOverrideCustomEventDefaultAccessSpecifier) && !bOverrideCustomEventDefaultAccessSpecifier)
		{
			CustomEventDefaultAccessSpecifier = ETsubasamusuAccessSpecifier::Public;
		}
#endif
		else if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UTsubasamusuUnrealAssistSettings, LlamaServerOptions))
		{
			SaveLlamaServerOptions();
		}
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

bool UTsubasamusuUnrealAssistSettings::LlamaServerOptionsContainSameElements() const
{
	for (const ULlamaServerOption* LlamaServerOption : LlamaServerOptions)
	{
		if (IsValid(LlamaServerOption))
		{
			auto IsSameOptionClass = [LlamaServerOption](const ULlamaServerOption* InLlamaServerOption)
			{
				return IsValid(InLlamaServerOption)
					&& LlamaServerOption != InLlamaServerOption
					&& LlamaServerOption->GetClass() == InLlamaServerOption->GetClass();
			};
			
			if (Algo::AnyOf(LlamaServerOptions, IsSameOptionClass))
			{
				return true;
			}
		}
	}
	
	return false;
}

FLlamaServerSettings UTsubasamusuUnrealAssistSettings::GetCurrentLlamaServerSettings() const
{
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 3, 0)
	return FLlamaServerSettings
	{
		.LlamaServerFilePath = LlamaServerFilePath,
		.ConfigLlamaServerOptions = ConfigLlamaServerOptions
	};
#else
	FLlamaServerSettings LlamaServerSettings;
	
	LlamaServerSettings.LlamaServerFilePath = LlamaServerFilePath;
	LlamaServerSettings.ConfigLlamaServerOptions = ConfigLlamaServerOptions;
	
	return LlamaServerSettings;
#endif
}

void UTsubasamusuUnrealAssistSettings::InitializeProperties()
{
	// General
	TickInterval = 0.f;
	
	// Comment Translator
	DeeplApiKey = FString();

	// Comment Generator
	bEnableStreamingCommentGeneration = true;
	bUseEditorLanguageForCommentGeneration = true;
	LanguageCultureNameForCommentGeneration = GetEditorLanguageCulture()->GetName();
	bIgnoreIsolatedNodesWhenGeneratingComments = true;
	bIgnoreCommentNodesWhenGeneratingComments = false;
	bUseToonFormatForCommentGeneration = true;
	CommentGenerationConditions = { TEXT("Length: Keep it brief but descriptive (one sentence or a short phrase).") };
	
	// Node Previewer
	bEnableNodePreview = false;
	bAlsoPreviewAdvancedView = false;
	NodePreviewScale = 1.f;
	
	// Access Specifier Initializer
	bOverrideVariableDefaultAccessSpecifier = false;
	VariableDefaultAccessSpecifier = ETsubasamusuAccessSpecifier::Public;
	bOverrideFunctionDefaultAccessSpecifier = false;
	FunctionDefaultAccessSpecifier = ETsubasamusuAccessSpecifier::Public;
#if CUSTOM_EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
	bCustomEventAccessSpecifierIsSupported = true;
	bOverrideCustomEventDefaultAccessSpecifier = false;
	CustomEventDefaultAccessSpecifier = ETsubasamusuAccessSpecifier::Public;
#else
	bCustomEventAccessSpecifierIsSupported = false;
#endif
	
	// LLM
	bStartLlamaServerOnEditorStartup = true;
	LlamaServerFilePath = FFilePath();
}

void UTsubasamusuUnrealAssistSettings::RestoreLlamaServerOptions()
{
	for (const FConfigLlamaServerOption& ConfigLlamaServerOption : ConfigLlamaServerOptions)
	{
		const UClass* LlamaServerOptionClass = ConfigLlamaServerOption.SoftClassPath.TryLoadClass<ULlamaServerOption>();
		ULlamaServerOption* LlamaServerOption = NewObject<ULlamaServerOption>(GetTransientPackage(), LlamaServerOptionClass);
    
		LlamaServerOption->SetArgument(ConfigLlamaServerOption.Argument);
		LlamaServerOptions.Add(LlamaServerOption);
	}
}

void UTsubasamusuUnrealAssistSettings::SaveLlamaServerOptions()
{
	ConfigLlamaServerOptions.Empty();
			
	for (const ULlamaServerOption* LlamaServerOption : LlamaServerOptions)
	{
		if (IsValid(LlamaServerOption))
		{
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 3, 0)
			FConfigLlamaServerOption ConfigLlamaServerOption
			{
				.SoftClassPath = FSoftClassPath(LlamaServerOption->GetClass()),
				.Argument = LlamaServerOption->GetArgument(true),
			};
#else
			FConfigLlamaServerOption ConfigLlamaServerOption;
			
			ConfigLlamaServerOption.SoftClassPath = FSoftClassPath(LlamaServerOption->GetClass());
			ConfigLlamaServerOption.Argument = LlamaServerOption->GetArgument(true);
#endif
			
			ConfigLlamaServerOptions.Add(ConfigLlamaServerOption);
		}
	}
			
	SaveConfig();
}

void UTsubasamusuUnrealAssistSettings::RegisterEditorLanguageChangedEvent()
{
	EditorLanguageChangedHandle = FInternationalization::Get().OnCultureChanged().AddLambda([this]()
	{
		if (bUseEditorLanguageForCommentGeneration)
		{
			MakeCommentGenerationLanguageSameAsEditorLanguage();
		}
	});
}

void UTsubasamusuUnrealAssistSettings::UnregisterEditorLanguageChangedEvent() const
{
	if (EditorLanguageChangedHandle.IsValid())
	{
		FInternationalization::Get().OnCultureChanged().Remove(EditorLanguageChangedHandle);
	}
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
