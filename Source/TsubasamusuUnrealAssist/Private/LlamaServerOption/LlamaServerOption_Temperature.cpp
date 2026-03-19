// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "LlamaServerOption_Temperature.h"

FString ULlamaServerOption_Temperature::GetParameter() const
{
	return TEXT("temperature");
}

void ULlamaServerOption_Temperature::SetArgument(const FString& InArgument)
{
	Temperature = FCString::Atof(*InArgument);
}

FString ULlamaServerOption_Temperature::GetArgument(const bool bForConfig) const
{
	return FString::SanitizeFloat(Temperature);
}

bool ULlamaServerOption_Temperature::IsValidArgument() const
{
	return 0.f <= Temperature && Temperature <= 1.f;
}
