// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "LlamaServerOption.h"
#include "LlamaServerOption_Model.generated.h"

/* LLM file path to load. */
UCLASS(DisplayName = "Model")
class ULlamaServerOption_Model : public ULlamaServerOption
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
	/* LLM file path to load. */
	UPROPERTY(EditAnywhere, meta = (DisplayName = "LLM File Path", FilePathFilter = "gguf"))
	FFilePath LlmFilePath;
};
