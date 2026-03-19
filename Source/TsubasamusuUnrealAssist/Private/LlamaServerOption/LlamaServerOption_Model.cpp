// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "LlamaServerOption_Model.h"

FString ULlamaServerOption_Model::GetParameter() const
{
	return TEXT("model");
}

void ULlamaServerOption_Model::SetArgument(const FString& InArgument)
{
	LlmFilePath.FilePath = InArgument;
}

FString ULlamaServerOption_Model::GetArgument(const bool bForConfig) const
{
	return LlmFilePath.FilePath;
}

bool ULlamaServerOption_Model::IsValidArgument() const
{
	return FPaths::FileExists(LlmFilePath.FilePath);
}
