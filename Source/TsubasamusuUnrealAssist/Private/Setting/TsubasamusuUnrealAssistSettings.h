// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "TsubasamusuUnrealAssistSettings.generated.h"

UCLASS(config = EditorPerProjectUserSettings)
class UTsubasamusuUnrealAssistSettings final : public UObject
{
	GENERATED_BODY()

public:
	explicit UTsubasamusuUnrealAssistSettings(const FObjectInitializer& ObjectInitializer);

#pragma region Comment Translation
	/* The DeepL API key used to translate comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Translation", meta = (DisplayName = "DeepL API Key"))
	FString DeeplApiKey;
#pragma endregion

#pragma region Comment Generation
	/* The OpenAI API key used to generate comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generation", meta = (DisplayName = "Open AI API Key"))
	FString OpenAiApiKey;
	
	/* The name of the GPT model used to generate comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generation", meta = (DisplayName = "GPT Model Name"))
	FString GptModelName;

	UPROPERTY(config)
	FString GptLanguageCultureName;
	
	/* Whether to ignore nodes that have no input pins, output pins, execution pins, etc. connected to them when generating comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generation")
	bool bIgnoreNodesDoNotHaveConnectedPins;
	
	/* Whether to ignore comment nodes contained within a comment node when generating comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generation")
	bool bIgnoreCommentNodes;
	
	/* Conditions that AI must adhere to when generating comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generation")
	TArray<FString> CommentGenerationConditions;
#pragma endregion
	
	FCulturePtr GetGptLanguageCulture() const;
	void SetGptLanguageCulture(const FCulturePtr& NewGptLanguageCulture);
	
	static UTsubasamusuUnrealAssistSettings* GetSettingsChecked();
};
