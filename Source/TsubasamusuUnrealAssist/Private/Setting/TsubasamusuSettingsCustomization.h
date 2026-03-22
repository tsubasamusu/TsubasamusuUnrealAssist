// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class ULlmManager;

class FTsubasamusuSettingsCustomization final : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> Create();
	virtual void CustomizeDetails(IDetailLayoutBuilder& InDetailLayoutBuilder) override;

private:
	static void AddButtonToApplyRecommendedEditorSettings(IDetailLayoutBuilder& InDetailLayoutBuilder);
	static void AddRestartLlamaServerMessage(IDetailLayoutBuilder& InDetailLayoutBuilder);
	static void AddCommentGenerationLanguageProperty(IDetailLayoutBuilder& InDetailLayoutBuilder);
	
	static EVisibility GetRestartLlamaServerMessageVisibility(const ULlmManager* InLlmManager);
	static FText GetRestartLlamaServerMessageText(const ULlmManager* InLlmManager);
	
	static void ChangePropertyDisplayAsPassword(IDetailLayoutBuilder& InDetailLayoutBuilder, const FName& InCategoryName, const FName& InPropertyName);
};
