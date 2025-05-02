// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "TsubasamusuUnrealAssistSettings.generated.h"

enum class ECommentNodeType;

UCLASS(config = EditorPerProjectUserSettings)
class TSUBASAMUSUUNREALASSIST_API UTsubasamusuUnrealAssistSettings final : public UObject
{
	GENERATED_BODY()

public:
	explicit UTsubasamusuUnrealAssistSettings(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere, config, Category = CommentNode)
	ECommentNodeType CommentNodeType;

	static UTsubasamusuUnrealAssistSettings* GetSettingsChecked();
};
