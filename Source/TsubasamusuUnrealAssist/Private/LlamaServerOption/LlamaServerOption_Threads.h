// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "LlamaServerOption.h"
#include "LlamaServerOption_Threads.generated.h"

/* The number of CPU threads to use during generation. */
UCLASS(DisplayName = "Threads")
class ULlamaServerOption_Threads : public ULlamaServerOption
{
	GENERATED_BODY()
	
public:
	//~ Begin ULlamaServerOption Interface
	virtual FString GetParameter() const override;
	
	virtual void SetArgument(const FString& InArgument) override;
	virtual FString GetArgument(const bool bForConfig = false) const override;
	//~ End ULlamaServerOption Interface
	
private:
	/* The number of CPU threads to use during generation. */
	UPROPERTY(EditAnywhere, meta = (DisplayName = "CPU Threads Number"))
	int32 CpuThreadsNumber = -1;
};
