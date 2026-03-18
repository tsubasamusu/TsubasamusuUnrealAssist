// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "Setting/TsubasamusuUnrealAssistSettings.h"
#include "TsubasamusuUnrealAssistModule.h"
#include "Algo/AnyOf.h"
#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h"
#include "InstancedObject/LlamaServerOption.h"
#include "Type/TsubasamusuUnrealAssistMacros.h"
#include "Type/TsubasamusuUnrealAssistStructs.h"

UTsubasamusuUnrealAssistSettings::UTsubasamusuUnrealAssistSettings(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
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
		FTsubasamusuUnrealAssistModule& TsubasamusuUnrealAssist = FModuleManager::LoadModuleChecked<FTsubasamusuUnrealAssistModule>(TEXT("TsubasamusuUnrealAssist"));
	
		if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UTsubasamusuUnrealAssistSettings, bUseEditorLanguageForCommentGeneration) && bUseEditorLanguageForCommentGeneration)
		{
			MakeCommentGenerationLanguageSameAsEditorLanguage();
		}
		else if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UTsubasamusuUnrealAssistSettings, TickInterval))
		{
			TsubasamusuUnrealAssist.ReregisterTicker();
		}
		else if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UTsubasamusuUnrealAssistSettings, bEnableNodePreview))
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

void UTsubasamusuUnrealAssistSettings::MakeCommentGenerationLanguageSameAsEditorLanguage()
{
	const FCultureRef EditorLanguageCulture = GetEditorLanguageCulture();
	SetCommentGenerationLanguageCulture(EditorLanguageCulture);
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

FLlmSettings UTsubasamusuUnrealAssistSettings::GetCurrentLlmSettings() const
{
	return FLlmSettings
	{
		.LlamaServerFilePath = LlamaServerFilePath,
		.ConfigLlamaServerOptions = ConfigLlamaServerOptions
	};
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
			FConfigLlamaServerOption ConfigLlamaServerOption
			{
				.SoftClassPath = FSoftClassPath(LlamaServerOption->GetClass()),
				.Argument = LlamaServerOption->GetArgument(),
			};
					
			ConfigLlamaServerOptions.Add(ConfigLlamaServerOption);
		}
	}
			
	SaveConfig();
}

FCultureRef UTsubasamusuUnrealAssistSettings::GetEditorLanguageCulture()
{
	return FInternationalization::Get().GetCurrentLanguage();
}
