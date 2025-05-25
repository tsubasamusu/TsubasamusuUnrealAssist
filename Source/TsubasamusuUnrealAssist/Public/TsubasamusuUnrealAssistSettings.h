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

	UPROPERTY(EditAnywhere, config, Category = "General")
	bool bEnableBlueprintSuggestion;
	
	UPROPERTY(EditAnywhere, config, Category = "Comment Node")
	ECommentNodeType CommentNodeType;
	
	UPROPERTY(EditAnywhere, config, Category = "Comment Node", meta = (DisplayName = "DeepL API Key"))
	FString DeeplApiKey;
	
	UPROPERTY(EditAnywhere, config, Category = "Comment Node|Gaming Comment Node", meta = (Units = "Seconds", DisplayName = "Color Animation Duration", ClampMin = "0.0"))
	float GamingColorAnimationDuration;

	UPROPERTY(EditAnywhere, config, Category = "Comment Node|Gaming Comment Node", meta = (Units = "Seconds", DisplayName = "Scale Animation Duration", ClampMin = "0.0"))
	float GamingScaleAnimationDuration;

	UPROPERTY(EditAnywhere, config, Category = "Comment Node|Gaming Comment Node", meta = (Units = "DegreesPerSecond", DisplayName = "Rotating Angle Per Seconds"))
	float GamingCommentNodeRotatingAnglePerSeconds;

	UPROPERTY(EditAnywhere, config, Category = "Comment Node|Gaming Comment Node", meta = (DisplayName = "Max Node Scale", ClampMin = "0.0"))
	float MaxGamingCommentNodeScale;
	
	UPROPERTY(EditAnywhere, config, Category = "Comment Node|PONG Comment Node", meta = (DisplayName = "PONG Background Color"))
	FLinearColor PongCommentNodeColor;
	
	UPROPERTY(EditAnywhere, config, Category = "Comment Node|PONG Comment Node", meta = (DisplayName = "PONG UI Color"))
	FLinearColor PongUiColor;
	
	UPROPERTY(EditAnywhere, config, Category = "Comment Node|PONG Comment Node", meta = (DisplayName = "Ball Size"))
	FVector2D PongBallImageSize;

	UPROPERTY(EditAnywhere, config, Category = "Comment Node|PONG Comment Node", meta = (DisplayName = "Slider Length Multiplier", ClampMin = "0.1", ClampMax = "1.0"))
	float PongSliderLengthMultiplier;

	UPROPERTY(EditAnywhere, config, Category = "Comment Node|PONG Comment Node", meta = (DisplayName = "Ball Speed", ClampMin = "0.0"))
	float PongBallSpeed;

	UPROPERTY(EditAnywhere, config, Category = "Comment Node|PONG Comment Node", meta = (Units = "Seconds", DisplayName = "Interval Seconds", ClampMin = "0.0", ToolTip = "The time between either player scoring and the ball being fired again."))
	float PongIntervalSeconds;
	
	UPROPERTY(EditAnywhere, config, Category = "Comment Node|PONG Comment Node", meta = (ToolTip = "Make the left scroll bar move automatically. You will not be able to control the left scroll bar."))
	bool bMakeLeftScrollBarNPC;
	
	static UTsubasamusuUnrealAssistSettings* GetSettingsChecked();
};
