// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "SWarningOrErrorBox.h"

class IDetailLayoutBuilder;

class FTsubasamusuSettingsCustomization final : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> Create();
	virtual void CustomizeDetails(IDetailLayoutBuilder& InDetailLayoutBuilder) override;

private:
	static void AddButtonToApplyRecommendedEditorSettings(IDetailLayoutBuilder& InDetailLayoutBuilder);
	static void AddRestartLlamaServerMessage(IDetailLayoutBuilder& InDetailLayoutBuilder);
	
	static EVisibility GetRestartLlamaServerMessageVisibility();
	static EMessageStyle GetRestartLlamaServerMessageStyle();
	static FText GetRestartLlamaServerButtonText();
	
    //TODO: Delete these functions calling
	static void ChangePropertyDisplayAsPassword(IDetailLayoutBuilder& InDetailLayoutBuilder, const FName& InCategoryName, const FName& InPropertyName);
	static void AddCommentGenerationLanguageProperty(IDetailLayoutBuilder& InDetailLayoutBuilder);
	static void AddGptModelsDocumentButton(IDetailLayoutBuilder& InDetailLayoutBuilder);
};
