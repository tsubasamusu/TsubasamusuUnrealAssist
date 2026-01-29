// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "TsubasamusuUnrealAssistSettings.generated.h"

UCLASS(config = EditorPerProjectUserSettings)
class TSUBASAMUSUUNREALASSIST_API UTsubasamusuUnrealAssistSettings final : public UObject
{
	GENERATED_BODY()

public:
	
	explicit UTsubasamusuUnrealAssistSettings(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, config, Category = "Comment Translation", meta = (DisplayName = "DeepL API Key"))
	FString DeeplApiKey;
	
	UPROPERTY(EditAnywhere, config, Category = "Comment Generation", meta = (DisplayName = "Open AI API Key"))
	FString OpenAiApiKey;
	
	UPROPERTY(EditAnywhere, config, Category = "Comment Generation", meta = (DisplayName = "GPT Model Name"))
	FString GptModelName;
	
	UPROPERTY(config)
	FString GptLanguageCultureName;
	
	/* Whether to ignore nodes that have no input pins, output pins, execution pins, etc. connected to them when generating comments */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generation")
	bool bIgnoreNodesDoNotHaveConnectedPins;
	
	/* Whether to ignore comment nodes contained within a comment node when generating comments */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generation")
	bool bIgnoreCommentNodes;
	
	/* Conditions that AI must adhere to when generating comments */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generation")
	TArray<FString> CommentGenerationConditions;

	FCulturePtr GetGptLanguageCulture() const;
	void SetGptLanguageCulture(const FCulturePtr& InGptLanguageCulture);
	
	static UTsubasamusuUnrealAssistSettings* GetSettingsChecked();
};
