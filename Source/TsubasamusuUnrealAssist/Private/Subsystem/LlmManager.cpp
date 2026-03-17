// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "LlmManager.h"
#include "Debug/TsubasamusuLogUtility.h"
#include "InstancedObject/LlamaServerOption.h"
#include "Setting/EditorSettingsUtility.h"
#include "Setting/TsubasamusuUnrealAssistSettings.h"

void ULlmManager::Initialize(FSubsystemCollectionBase& InSubsystemCollectionBase)
{
	LlamaServerStatus = ELlamaServerStatus::NotStartedYet;
	
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
	
	if (TsubasamusuUnrealAssistSettings->bStartLlamaServerOnEditorStartup)
	{
		StartLlamaServer();
	}
}

void ULlmManager::Deinitialize()
{
	StopLlamaServer();
}

void ULlmManager::RestartLlamaServer()
{
	StopLlamaServer();
	StartLlamaServer();
}

void ULlmManager::StartLlamaServer()
{
	FString ErrorMessage;
	
	ON_SCOPE_EXIT
	{
		if (!ErrorMessage.IsEmpty())
		{
			TUA_ERROR(TEXT("%s"), *ErrorMessage);
			LastAppliedLlmSettings.Reset();
		}
		
		LlamaServerStatus = ErrorMessage.IsEmpty() ? ELlamaServerStatus::SuccessfullyStarted : ELlamaServerStatus::FailedToStart;
	};
	
	if (FPlatformProcess::IsProcRunning(LlamaServerProcessHandle))
	{
		ErrorMessage = TEXT("Llama server is already running. Please stop the server and try again.");
		return;
	}
	
	const UTsubasamusuUnrealAssistSettings* TsubasamusuUnrealAssistSettings = FEditorSettingsUtility::GetSettingsChecked<UTsubasamusuUnrealAssistSettings>();
	const FString LlamaServerFilePath = TsubasamusuUnrealAssistSettings->LlamaServerFilePath.FilePath;
	
	if (!LlamaServerFilePath.IsEmpty())
	{
		if (!FPaths::FileExists(TsubasamusuUnrealAssistSettings->LlamaServerFilePath.FilePath))
		{
			ErrorMessage = FString::Printf(TEXT("Llama server file not found. The file path is \"%s.\""), *LlamaServerFilePath);
			return;
		}
	
		if (TsubasamusuUnrealAssistSettings->LlamaServerOptionsContainSameElements())
		{
			ErrorMessage = TEXT("There are duplicate elements in the command-line arguments used when starting the Llama server.");
			return;
		}
	
		FString Arguments;
		
		for (const ULlamaServerOption* LlamaServerOption : TsubasamusuUnrealAssistSettings->LlamaServerOptions)
		{
			if (IsValid(LlamaServerOption))
			{
				if (!LlamaServerOption->IsValidArgument())
				{
					ErrorMessage = FString::Printf(TEXT("The argument \"%s\" for \"%s\" parameter is invalid."), *LlamaServerOption->GetArgument(), *LlamaServerOption->GetParameter());
					return;
				}
		
				const FString ParameterSymbol = LlamaServerOption->IsLongParameter() ? TEXT("--") : TEXT("-");
				const FString Argument = LlamaServerOption->GetArgument(true).IsEmpty() ? FString() : FString::Printf(TEXT(" %s"), *LlamaServerOption->GetArgument(true));
				Arguments += FString::Printf(TEXT(" %s%s%s"), *ParameterSymbol, *LlamaServerOption->GetParameter(), *Argument);
			}
		}
	
		LlamaServerProcessHandle = FPlatformProcess::CreateProc(*TsubasamusuUnrealAssistSettings->LlamaServerFilePath.FilePath, *Arguments, true, false, false, nullptr, 0, nullptr, nullptr);
		LastAppliedLlmSettings = TsubasamusuUnrealAssistSettings->GetCurrentLlmSettings();
	}
}

void ULlmManager::StopLlamaServer()
{
	if (FPlatformProcess::IsProcRunning(LlamaServerProcessHandle))
	{
		FPlatformProcess::TerminateProc(LlamaServerProcessHandle);
		FPlatformProcess::CloseProc(LlamaServerProcessHandle);
		LlamaServerProcessHandle.Reset();
		
		LlamaServerStatus = ELlamaServerStatus::NotStartedYet;
	}
}

FString ULlmManager::GetLlamaServerPort() const
{
	auto IsPortOption = [](const FConfigLlamaServerOption& InConfigLlamaServerOption)
	{
		return InConfigLlamaServerOption.SoftClassPath == FSoftClassPath(ULlamaServerOption_Port::StaticClass());
	};
	
	const FConfigLlamaServerOption* PortOptionPtr = LastAppliedLlmSettings.ConfigLlamaServerOptions.FindByPredicate(IsPortOption);
	const FString DefaultPort = FString::FromInt(ULlamaServerOption_Port::DefaultLlamaServerPort);
	
	return PortOptionPtr ? PortOptionPtr->Argument : DefaultPort;
}
