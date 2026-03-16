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
	
public:
	//~ Begin ULlamaServerOption Interface
	virtual FString GetParameter() const override;
	
	virtual void SetArgument(const FString& InArgument) override;
	virtual FString GetArgument() const override;
	
	virtual bool IsValidArgument() const override;
	//~ End ULlamaServerOption Interface
	
private:
	/* LLM file path to load. */
	UPROPERTY(EditAnywhere, meta = (DisplayName = "LLM File Path", FilePathFilter = "gguf"))
	FFilePath LlmFilePath;
};

/* The port number for the Llama server. */
UCLASS(DisplayName = "Port")
class ULlamaServerOption_Port : public ULlamaServerOption
{
	GENERATED_BODY()
	
public:
	//~ Begin ULlamaServerOption Interface
	virtual FString GetParameter() const override;
	
	virtual void SetArgument(const FString& InArgument) override;
	virtual FString GetArgument() const override;
	
	virtual bool IsValidArgument() const override;
	//~ End ULlamaServerOption Interface
	
private:
	/* The port number for the Llama server. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1024", ClampMax = "65535"))
	int32 LlamaServerPort = 8080;
};

/* The size of the prompt context. If set to 0, the context size is loaded from the model. */
UCLASS(DisplayName = "ContextSize")
class ULlamaServerOption_ContextSize : public ULlamaServerOption
{
	GENERATED_BODY()
	
public:
	//~ Begin ULlamaServerOption Interface
	virtual FString GetParameter() const override;
	
	virtual void SetArgument(const FString& InArgument) override;
	virtual FString GetArgument() const override;
	
	virtual bool IsValidArgument() const override;
	//~ End ULlamaServerOption Interface
	
private:
	/* The size of the prompt context. If set to 0, the context size is loaded from the model. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	int32 ContextSize = 0;
};
