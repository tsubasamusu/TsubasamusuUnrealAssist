// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "LlamaServerOption_EnableWebUi.h"

FString ULlamaServerOption_EnableWebUi::GetParameter() const
{
	return bEnableWebUi ? TEXT("webui") : TEXT("no-webui");
}

void ULlamaServerOption_EnableWebUi::SetArgument(const FString& InArgument)
{
	bEnableWebUi = InArgument == TEXT("true");
}

FString ULlamaServerOption_EnableWebUi::GetArgument(const bool bForConfig) const
{
	if (bForConfig)
	{
		return bEnableWebUi ? TEXT("true") : TEXT("false");
	}
	
	return FString();
}
