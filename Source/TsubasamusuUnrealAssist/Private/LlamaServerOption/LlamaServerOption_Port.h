// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "LlamaServerOption.h"
#include "LlamaServerOption_Port.generated.h"

/* The port number for the Llama server. */
UCLASS(DisplayName = "Port")
class ULlamaServerOption_Port : public ULlamaServerOption
{
	GENERATED_BODY()
	
public:
	//~ Begin ULlamaServerOption Interface
	virtual FString GetParameter() const override;
	
	virtual void SetArgument(const FString& InArgument) override;
	virtual FString GetArgument(const bool bForConfig = false) const override;
	
	virtual bool IsValidArgument() const override;
	//~ End ULlamaServerOption Interface

	static constexpr int32 DefaultLlamaServerPort = 8080;
	
private:
	/* The port number for the Llama server. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1024", ClampMax = "65535"))
	int32 LlamaServerPort = DefaultLlamaServerPort;
};
