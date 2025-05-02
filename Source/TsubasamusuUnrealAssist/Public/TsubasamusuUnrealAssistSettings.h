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
	
	UPROPERTY(EditAnywhere, config, Category = "Comment Node")
	ECommentNodeType CommentNodeType;
	
	UPROPERTY(EditAnywhere, config, Category = "Comment Node|Gaming Comment Node", meta = (Units = "Seconds"))
	float ColorAnimationDuration;

	UPROPERTY(EditAnywhere, config, Category = "Comment Node|Gaming Comment Node", meta = (Units = "Seconds"))
	float ScaleAnimationDuration;

	UPROPERTY(EditAnywhere, config, Category = "Comment Node|Gaming Comment Node", meta = (Units = "DegreesPerSecond"))
	float RotatingAnglePerSeconds;

	UPROPERTY(EditAnywhere, config, Category = "Comment Node|Gaming Comment Node")
	float MaxCommentNodeScale;
	
	static UTsubasamusuUnrealAssistSettings* GetSettingsChecked();
};
