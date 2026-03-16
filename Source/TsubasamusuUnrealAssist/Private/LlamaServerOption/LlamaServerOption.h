// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "LlamaServerOption.generated.h"

UCLASS(EditInlineNew, CollapseCategories, Abstract)
class ULlamaServerOption : public UObject
{
	GENERATED_BODY()
	
public:
	virtual FString GetParameter() const PURE_VIRTUAL(ULlamaServerOption::GetParameter, return FString(););
	
	virtual void SetArgument(const FString& InArgument) PURE_VIRTUAL(ULlamaServerOption::SetArgument, );
	virtual FString GetArgument() const PURE_VIRTUAL(ULlamaServerOption::GetArgument, return FString(););
	
	virtual bool IsValidArgument() const PURE_VIRTUAL(ULlamaServerOption::IsValidArgument, return false;);
	
	virtual bool IsLongParameter() const { return true; };
};

/* LLM file path to load. */
UCLASS(DisplayName = "Model")
class ULlamaServerOption_Model : public ULlamaServerOption
{
	GENERATED_BODY()
	
private:
	/* LLM file path to load. */
	UPROPERTY(EditAnywhere, meta = (DisplayName = "LLM File Path", FilePathFilter = "gguf"))
	FFilePath LlmFilePath;
	
public:
	//~ Begin ULlamaServerOption Interface
	virtual FString GetParameter() const override;
	
	virtual void SetArgument(const FString& InArgument) override;
	virtual FString GetArgument() const override;
	
	virtual bool IsValidArgument() const override;
	//~ End ULlamaServerOption Interface
};
