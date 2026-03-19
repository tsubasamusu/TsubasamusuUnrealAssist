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
	virtual FString GetArgument(const bool bForConfig = false) const PURE_VIRTUAL(ULlamaServerOption::GetArgument, return FString(););
	
	virtual bool IsValidArgument() const { return true; }
	virtual bool IsLongParameter() const { return true; }
};
