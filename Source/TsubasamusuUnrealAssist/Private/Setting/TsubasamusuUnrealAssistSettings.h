// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Type/TsubasamusuUnrealAssistEnums.h"
#include "Type/TsubasamusuUnrealAssistStructs.h"
#include "TsubasamusuUnrealAssistSettings.generated.h"

class ULlamaServerOption;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSettingsPropertyChanged, const FName& /*InChangedPropertyName*/);

UCLASS(config = EditorPerProjectUserSettings)
class UTsubasamusuUnrealAssistSettings final : public UObject
{
	GENERATED_BODY()

public:
	explicit UTsubasamusuUnrealAssistSettings(const FObjectInitializer& InObjectInitializer);
	virtual ~UTsubasamusuUnrealAssistSettings() override;
	
	//~ Begin UObject Interface
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& InPropertyChangedEvent) override;
	//~ End UObject Interface

#pragma region General
	/* The tick interval of Tsubasamusu Unreal Assist. If set to 0 seconds, the tick processing will run every frame. */
	UPROPERTY(EditAnywhere, config, Category = "General", meta = (ClampMin = "0.0", Units = "s"))
	float TickInterval;
#pragma endregion

#pragma region Comment Translator
	/* The DeepL API key used to translate comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Translator", meta = (DisplayName = "DeepL API Key"))
	FString DeeplApiKey;
#pragma endregion

#pragma region Comment Generator
	/* Whether to use streaming generation when generating comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generator", meta = (DisplayName = "Enable Streaming Generation"))
	bool bEnableStreamingCommentGeneration;

	/* Whether to match the language of generated comments to the editor language. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generator", meta = (DisplayName = "Use Editor Language"))
	bool bUseEditorLanguageForCommentGeneration;

	/* Whether to ignore nodes that have no input pins, output pins, execution pins, etc. connected to them when generating comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generator", meta = (DisplayName = "Ignore Nodes Do Not Have Connected Pins"))
	bool bIgnoreIsolatedNodesWhenGeneratingComments;
	
	/* Whether to ignore comment nodes contained within a comment node when generating comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generator", meta = (DisplayName = "Ignore Comment Nodes"))
	bool bIgnoreCommentNodesWhenGeneratingComments;
	
	/* Whether the string format used to pass nodes information to LLM for comment generation should be TOON. If false, JSON is used instead. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generator", meta = (DisplayName = "Use TOON Format"))
	bool bUseToonFormatForCommentGeneration;
	
	/* Conditions that AI must adhere to when generating comments. */
	UPROPERTY(EditAnywhere, config, Category = "Comment Generator", meta = (DisplayName = "Conditions"))
	TArray<FString> CommentGenerationConditions;
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

#pragma region LLM
	/* Whether to start Llama server when the editor launches. */
	UPROPERTY(EditAnywhere, config, Category = "LLM")
	bool bStartLlamaServerOnEditorStartup;
	
	/* The file path of llama-server.exe. */
	UPROPERTY(EditAnywhere, config, Category = "LLM", meta = (FilePathFilter = "exe"))
	FFilePath LlamaServerFilePath;
	
	/* Command-line arguments used when starting the Llama server. */
	UPROPERTY(EditAnywhere, Category = "LLM", Instanced)
	TArray<ULlamaServerOption*> LlamaServerOptions;
#pragma endregion

	FCulturePtr GetCommentGenerationLanguageCulture() const;
	void SetCommentGenerationLanguageCulture(const FCulturePtr InCommentGenerationLanguageCulture);
	
	bool LlamaServerOptionsContainSameElements() const;
	FLlamaServerSettings GetCurrentLlamaServerSettings() const;
	
	FOnSettingsPropertyChanged OnSettingsPropertyChanged;
	
private:
	UPROPERTY(config)
	FString LanguageCultureNameForCommentGeneration;
	
	UPROPERTY(config)
	TArray<FConfigLlamaServerOption> ConfigLlamaServerOptions;
	
	void InitializeProperties();
	
	void RestoreLlamaServerOptions();
	void SaveLlamaServerOptions();
	
	void RegisterEditorLanguageChangedEvent();
	void UnregisterEditorLanguageChangedEvent() const;
	
	void MakeCommentGenerationLanguageSameAsEditorLanguage();
	static FCultureRef GetEditorLanguageCulture();
	
	FDelegateHandle PostEngineInitHandle;
	FDelegateHandle EditorLanguageChangedHandle;
};
