// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "LlamaServerOption.h"
#include "LlamaServerOption_ContextSize.generated.h"

/* The size of the prompt context. If set to 0, the context size is loaded from the model. */
UCLASS(DisplayName = "ContextSize")
class ULlamaServerOption_ContextSize : public ULlamaServerOption
{
	GENERATED_BODY()
	
public:
	//~ Begin ULlamaServerOption Interface
	virtual FString GetParameter() const override;
	
	virtual void SetArgument(const FString& InArgument) override;
	virtual FString GetArgument(const bool bForConfig = false) const override;
	
	virtual bool IsValidArgument() const override;
	//~ End ULlamaServerOption Interface
	
private:
	/* The size of the prompt context. If set to 0, the context size is loaded from the model. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	int32 ContextSize = 0;
};
