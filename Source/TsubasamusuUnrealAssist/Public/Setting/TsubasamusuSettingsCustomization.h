// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class IDetailLayoutBuilder;

class TSUBASAMUSUUNREALASSIST_API FTsubasamusuSettingsCustomization final : public IDetailCustomization
{
public:
	
	static TSharedRef<IDetailCustomization> Create();
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	
	static void ChangeOpenAiApiKeyPropertyDisplayAsPassword(IDetailLayoutBuilder& DetailLayout);
	static void AddGptLanguageProperty(IDetailLayoutBuilder& DetailLayout);
	static void AddGptModelsDocumentButton(IDetailLayoutBuilder& DetailLayout);
};
