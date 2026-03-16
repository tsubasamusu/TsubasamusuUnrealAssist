// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Type/TsubasamusuUnrealAssistEnums.h"
#include "TsubasamusuUnrealAssistSettings.generated.h"

struct FConfigLlamaServerOption;
class ULlamaServerOption;

UCLASS(config = EditorPerProjectUserSettings)
class UTsubasamusuUnrealAssistSettings final : public UObject
{
	GENERATED_BODY()

public:
	explicit UTsubasamusuUnrealAssistSettings(const FObjectInitializer& InObjectInitializer);
	
	//~ Begin UObject Interface
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& InPropertyChangedEvent) override;
	//~ End UObject Interface

#pragma region General
	/* The tick interval of Tsubasamusu Unreal Assist. If set to 0 seconds, the tick processing will run every frame. */
	UPROPERTY(EditAnywhere, config, Category = "General", meta = (ClampMin = "0.0", Units = "s"))
	float TickInterval;
	
	/* Command line arguments used when starting Llama server. */
	UPROPERTY(EditAnywhere, Category = "General", Instanced)
	TArray<ULlamaServerOption*> LlamaServerOptions;
#pragma endregion

#pragma region Comment Translator
	/* The DeepL API key used to translate comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Translator", meta = (DisplayName = "DeepL API Key"))
	FString DeeplApiKey;
#pragma endregion

#pragma region Comment Generator
	/* The OpenAI API key used to generate comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generator", meta = (DisplayName = "OpenAI API Key"))
	FString OpenAiApiKey;
	
	/* The name of the GPT model used to generate comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generator", meta = (DisplayName = "GPT Model Name"))
	FString GptModelName;

	/* Whether to match the language of generated comments to the editor language. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generator", meta = (DisplayName = "Use Editor Language"))
	bool bUseEditorLanguageForCommentGeneration;

	/* Whether to ignore nodes that have no input pins, output pins, execution pins, etc. connected to them when generating comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generator", meta = (DisplayName = "Ignore Nodes Do Not Have Connected Pins"))
	bool bIgnoreIsolatedNodesWhenGeneratingComments;
	
	/* Whether to ignore comment nodes contained within a comment node when generating comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generator", meta = (DisplayName = "Ignore Comment Nodes"))
	bool bIgnoreCommentNodesWhenGeneratingComments;
	
	/* Conditions that AI must adhere to when generating comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generator", meta = (DisplayName = "Conditions"))
	TArray<FString> CommentGenerationConditions;
	
	/* Whether the string format used to pass nodes information to GPT for comment generation should be TOON. If false, JSON is used instead. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generator", AdvancedDisplay, meta = (DisplayName = "Use TOON Format"))
	bool bUseToonFormatForCommentGeneration;
#pragma endregion

#pragma region Node Previewer
	/* Whether to display a preview of the node being hovered over in the node search window. */
	UPROPERTY(EditAnywhere, config, Category = "Node Previewer")
	bool bEnableNodePreview;
	
	/* Whether to display pins that are hidden by default in the node preview. For example, pins such as TextColor, Duration, and Key of the PrintString node. */
	UPROPERTY(EditAnywhere, config, Category = "Node Previewer", meta = (EditCondition = "bEnableNodePreview"))
	bool bAlsoPreviewAdvancedView;
	
	/* The scale of node preview. */
	UPROPERTY(EditAnywhere, config, Category = "Node Previewer", meta = (DisplayName = "Preview Scale", ClampMin = "0.0", EditCondition = "bEnableNodePreview"))
	float NodePreviewScale;
#pragma endregion

#pragma region Access Specifier Initializer
	/* Whether to change the default value of variable access specifier. */
	UPROPERTY(EditAnywhere, config, Category = "Access Specifier Initializer")
	bool bOverrideVariableDefaultAccessSpecifier;
	
	/* The default value of variable access specifier. */
	UPROPERTY(EditAnywhere, config, Category = "Access Specifier Initializer", meta = (ValidEnumValues = "Private, Public", EditCondition = "bOverrideVariableDefaultAccessSpecifier"))
	ETsubasamusuAccessSpecifier VariableDefaultAccessSpecifier;
	
	/* Whether to change the default value of function access specifier. */
	UPROPERTY(EditAnywhere, config, Category = "Access Specifier Initializer")
	bool bOverrideFunctionDefaultAccessSpecifier;
	
	/* The default value of function access specifier. */
	UPROPERTY(EditAnywhere, config, Category = "Access Specifier Initializer", meta = (ValidEnumValues = "Private, Protected, Public", EditCondition = "bOverrideFunctionDefaultAccessSpecifier"))
	ETsubasamusuAccessSpecifier FunctionDefaultAccessSpecifier;
	
private:
	UPROPERTY()
	bool bCustomEventAccessSpecifierIsSupported;
	
public:
	/* Whether to change the default value of custom event access specifier. */
	UPROPERTY(EditAnywhere, config, Category = "Access Specifier Initializer", meta = (EditCondition = "bCustomEventAccessSpecifierIsSupported", HideEditConditionToggle))
	bool bOverrideCustomEventDefaultAccessSpecifier;
	
	/* The default value of custom event access specifier. */
	UPROPERTY(EditAnywhere, config, Category = "Access Specifier Initializer", meta = (ValidEnumValues = "Private, Protected, Public", EditCondition = "bOverrideCustomEventDefaultAccessSpecifier && bCustomEventAccessSpecifierIsSupported"))
	ETsubasamusuAccessSpecifier CustomEventDefaultAccessSpecifier;
#pragma endregion

	FCulturePtr GetCommentGenerationLanguageCulture() const;
	void SetCommentGenerationLanguageCulture(const FCulturePtr InCommentGenerationLanguageCulture);
	void MakeCommentGenerationLanguageSameAsEditorLanguage();
	
private:
	UPROPERTY(config)
	FString LanguageCultureNameForCommentGeneration;
	
	UPROPERTY(config)
	TArray<FConfigLlamaServerOption> ConfigLlamaServerOptions;
	
	void RestoreLlamaServerOptions();
	void SaveLlamaServerOptions();
	
	static FCultureRef GetEditorLanguageCulture();
};
