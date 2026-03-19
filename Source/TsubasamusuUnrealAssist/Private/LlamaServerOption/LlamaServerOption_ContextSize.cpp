// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "LlamaServerOption_ContextSize.h"

FString ULlamaServerOption_ContextSize::GetParameter() const
{
	return TEXT("ctx-size");
}

void ULlamaServerOption_ContextSize::SetArgument(const FString& InArgument)
{
	ContextSize = FCString::Atoi(*InArgument);
}

FString ULlamaServerOption_ContextSize::GetArgument(const bool bForConfig) const
{
	return FString::FromInt(ContextSize);
}

bool ULlamaServerOption_ContextSize::IsValidArgument() const
{
	return ContextSize >= 0;
}
