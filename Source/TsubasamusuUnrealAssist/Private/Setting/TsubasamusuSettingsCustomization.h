// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class IDetailLayoutBuilder;

class FTsubasamusuSettingsCustomization final : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> Create();
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	static void ChangePropertyDisplayAsPassword(IDetailLayoutBuilder& InDetailLayoutBuilder, const FName& CategoryName, const FName& PropertyName);
	static void AddCommentGenerationLanguageProperty(IDetailLayoutBuilder& InDetailLayoutBuilder);
	static void AddGptModelsDocumentButton(IDetailLayoutBuilder& InDetailLayoutBuilder);
	static void AddButtonToApplyRecommendedEditorSettings(IDetailLayoutBuilder& InDetailLayoutBuilder);
};
