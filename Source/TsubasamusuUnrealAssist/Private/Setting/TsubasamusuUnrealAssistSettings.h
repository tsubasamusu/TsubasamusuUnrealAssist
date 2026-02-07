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
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#pragma region General
	/* The tick interval of Tsubasamusu Unreal Assist. If set to 0 seconds, the tick processing will run every frame. */
	UPROPERTY(EditAnywhere, config, Category = "General", meta = (ClampMin = "0.0", ClampMax = "1.0", Units = "s"))
	float TickInterval;
#pragma endregion

#pragma region Comment Translation
	/* The DeepL API key used to translate comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Translation", meta = (DisplayName = "DeepL API Key"))
	FString DeeplApiKey;
#pragma endregion

#pragma region Comment Generation
	/* The OpenAI API key used to generate comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generation", meta = (DisplayName = "OpenAI API Key"))
	FString OpenAiApiKey;
	
	/* The name of the GPT model used to generate comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generation", meta = (DisplayName = "GPT Model Name"))
	FString GptModelName;

	/* Whether to match the language of generated comments to the editor language. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generation", meta = (DisplayName = "Use Editor Language"))
	bool bUseEditorLanguageForCommentGeneration;

	UPROPERTY(config)
	FString LanguageCultureNameForCommentGeneration;
	
	/* Whether to ignore nodes that have no input pins, output pins, execution pins, etc. connected to them when generating comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generation", meta = (DisplayName = "Ignore Nodes Do Not Have Connected Pins"))
	bool bIgnoreIsolatedNodesWhenGeneratingComments;
	
	/* Whether to ignore comment nodes contained within a comment node when generating comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generation", meta = (DisplayName = "Ignore Comment Nodes"))
	bool bIgnoreCommentNodesWhenGeneratingComments;
	
	/* Whether the string format used to pass nodes information to GPT for comment generation should be TOON. If false, JSON is used instead. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generation", meta = (DisplayName = "Use TOON Format"))
	bool bUseToonFormatForCommentGeneration;
	
	/* Conditions that AI must adhere to when generating comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generation", meta = (DisplayName = "Conditions"))
	TArray<FString> CommentGenerationConditions;
#pragma endregion

#pragma region Node Preview
	/* Whether to display a preview of the node being hovered over in the node search window. */
	UPROPERTY(EditAnywhere, config, Category = "Node Preview")
	bool bEnableNodePreview;
#pragma endregion

	FCulturePtr GetCommentGenerationLanguageCulture() const;
	void SetCommentGenerationLanguageCulture(const FCulturePtr& NewCommentGenerationLanguageCulture);
	void MakeCommentGenerationLanguageSameAsEditorLanguage();
	
private:
	static FCultureRef GetEditorLanguageCulture();
};
