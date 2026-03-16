// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "LlamaServerOption.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

FString ULlamaServerOption_Model::GetParameter() const
{
	return TEXT("model");
}

void ULlamaServerOption_Model::SetArgument(const FString& InArgument)
{
	LlmFilePath.FilePath = InArgument;
}

FString ULlamaServerOption_Model::GetArgument(const bool bForCommandLineArguments) const
{
	return LlmFilePath.FilePath;
}

bool ULlamaServerOption_Model::IsValidArgument() const
{
	return FPaths::FileExists(LlmFilePath.FilePath);
}

FString ULlamaServerOption_Port::GetParameter() const
{
	return TEXT("port");
}

void ULlamaServerOption_Port::SetArgument(const FString& InArgument)
{
	LlamaServerPort = FCString::Atoi(*InArgument);
}

FString ULlamaServerOption_Port::GetArgument(const bool bForCommandLineArguments) const
{
	return FString::FromInt(LlamaServerPort);
}

bool ULlamaServerOption_Port::IsValidArgument() const
{
	bool bLlamaServerPortIsAvailable = false;
    
	if (1024 <= LlamaServerPort && LlamaServerPort <= 65535)
	{
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		FSocket* Socket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("LlamaServerTestSocket"));
		
		if (Socket)
		{
			const TSharedPtr<FInternetAddr> InternetAddress = SocketSubsystem->CreateInternetAddr();
 
			InternetAddress->SetAnyAddress();
			InternetAddress->SetPort(LlamaServerPort);
 
			bLlamaServerPortIsAvailable = Socket->Bind(*InternetAddress);
 
			Socket->Close();
			SocketSubsystem->DestroySocket(Socket);
		}
	}
    
	return bLlamaServerPortIsAvailable;
}

FString ULlamaServerOption_ContextSize::GetParameter() const
{
	return TEXT("ctx-size");
}

void ULlamaServerOption_ContextSize::SetArgument(const FString& InArgument)
{
	ContextSize = FCString::Atoi(*InArgument);
}

FString ULlamaServerOption_ContextSize::GetArgument(const bool bForCommandLineArguments) const
{
	return FString::FromInt(ContextSize);
}

bool ULlamaServerOption_ContextSize::IsValidArgument() const
{
	return ContextSize >= 0;
}

FString ULlamaServerOption_Threads::GetParameter() const
{
	return TEXT("threads");
}

void ULlamaServerOption_Threads::SetArgument(const FString& InArgument)
{
	CpuThreadsNumber = FCString::Atoi(*InArgument);
}

FString ULlamaServerOption_Threads::GetArgument(const bool bForCommandLineArguments) const
{
	return FString::FromInt(CpuThreadsNumber);
}

bool ULlamaServerOption_Threads::IsValidArgument() const
{
	return true;
}

FString ULlamaServerOption_EnableWebUi::GetParameter() const
{
	return bEnableWebUi ? TEXT("webui") : TEXT("no-webui");
}

void ULlamaServerOption_EnableWebUi::SetArgument(const FString& InArgument)
{
	bEnableWebUi = InArgument == TEXT("true");
}

FString ULlamaServerOption_EnableWebUi::GetArgument(const bool bForCommandLineArguments) const
{
	if (bForCommandLineArguments)
	{
		return FString();
	}
	
	return bEnableWebUi ? TEXT("true") : TEXT("false");
}

bool ULlamaServerOption_EnableWebUi::IsValidArgument() const
{
	return true;
}

FString ULlamaServerOption_Temperature::GetParameter() const
{
	return TEXT("temperature");
}

void ULlamaServerOption_Temperature::SetArgument(const FString& InArgument)
{
	Temperature = FCString::Atof(*InArgument);
}

FString ULlamaServerOption_Temperature::GetArgument(const bool bForCommandLineArguments) const
{
	return FString::SanitizeFloat(Temperature);
}

bool ULlamaServerOption_Temperature::IsValidArgument() const
{
	return 0.f <= Temperature && Temperature <= 1.f;
}
