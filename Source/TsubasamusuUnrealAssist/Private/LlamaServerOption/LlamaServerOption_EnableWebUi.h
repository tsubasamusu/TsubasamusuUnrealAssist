// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "LlamaServerOption.h"
#include "LlamaServerOption_EnableWebUi.generated.h"

/* Whether to enable the Web UI. */
UCLASS(DisplayName = "Enable Web UI")
class ULlamaServerOption_EnableWebUi : public ULlamaServerOption
{
	GENERATED_BODY()
	
public:
	//~ Begin ULlamaServerOption Interface
	virtual FString GetParameter() const override;
	
	virtual void SetArgument(const FString& InArgument) override;
	virtual FString GetArgument(const bool bForConfig = false) const override;
	//~ End ULlamaServerOption Interface
	
private:
	/* Whether to enable the Web UI. */
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Enable Web UI"))
	bool bEnableWebUi = false;
};
