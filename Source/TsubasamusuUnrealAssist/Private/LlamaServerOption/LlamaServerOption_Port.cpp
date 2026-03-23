// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "LlamaServerOption_Port.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

FString ULlamaServerOption_Port::GetParameter() const
{
	return TEXT("port");
}

void ULlamaServerOption_Port::SetArgument(const FString& InArgument)
{
	LlamaServerPort = FCString::Atoi(*InArgument);
}

FString ULlamaServerOption_Port::GetArgument(const bool bForConfig) const
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
