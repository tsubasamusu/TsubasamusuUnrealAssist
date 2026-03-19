// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "LlamaServerOption.h"
#include "Type/TsubasamusuUnrealAssistEnums.h"
#include "LlamaServerOption_GpuLayers.generated.h"

/* The number of layers to store in VRAM. */
UCLASS(DisplayName = "GPU Layers")
class ULlamaServerOption_GpuLayers : public ULlamaServerOption
{
	GENERATED_BODY()
	
public:
	//~ Begin UObject Interface
	virtual void PostEditChangeProperty(FPropertyChangedEvent& InPropertyChangedEvent) override;
	//~ End UObject Interface

	//~ Begin ULlamaServerOption Interface
	virtual FString GetParameter() const override;
	
	virtual void SetArgument(const FString& InArgument) override;
	virtual FString GetArgument(const bool bForConfig = false) const override;
	
	virtual bool IsValidArgument() const override;
	//~ End ULlamaServerOption Interface
	
private:
	/* The number type of layers to store in VRAM. */
	UPROPERTY(EditAnywhere, meta = (DisplayName = "GPU Layers"))
	EGpuLayers GpuLayers = EGpuLayers::Auto;
	
	/* The number of layers to store in VRAM. */
	UPROPERTY(EditAnywhere, meta = (DisplayName = "GPU Layers Number", ClampMin = "0", EditCondition = "GpuLayers == EGpuLayers::ExactNumber", EditConditionHides))
	int32 GpuLayersNumber = -2;
};
