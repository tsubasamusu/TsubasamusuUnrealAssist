// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "LlamaServerOption_Threads.h"

FString ULlamaServerOption_Threads::GetParameter() const
{
	return TEXT("threads");
}

void ULlamaServerOption_Threads::SetArgument(const FString& InArgument)
{
	CpuThreadsNumber = FCString::Atoi(*InArgument);
}

FString ULlamaServerOption_Threads::GetArgument(const bool bForConfig) const
{
	return FString::FromInt(CpuThreadsNumber);
}
