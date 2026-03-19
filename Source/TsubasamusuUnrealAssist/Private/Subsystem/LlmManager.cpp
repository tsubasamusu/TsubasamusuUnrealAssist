// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "LlmManager.h"
#include "HttpModule.h"
#include "Debug/TsubasamusuLogUtility.h"
#include "InstancedObject/LlamaServerOption.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
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

void ULlmManager::GenerateToken(const FString& InPrompt, const FOnTokenGenerated& InTokenGeneratedFunction, const bool bEnableStreaming) const
{
    const TSharedPtr<FJsonObject> MessageJsonObject = MakeShared<FJsonObject>();
	
    MessageJsonObject->SetStringField(TEXT("role"), TEXT("user"));
    MessageJsonObject->SetStringField(TEXT("content"), InPrompt);
	
	const TSharedPtr<FJsonObject> RequestJsonObject = MakeShared<FJsonObject>();
    const TArray<TSharedPtr<FJsonValue>> MessageJsonValues = { MakeShared<FJsonValueObject>(MessageJsonObject) };
    
    RequestJsonObject->SetArrayField(TEXT("messages"), MessageJsonValues);
    RequestJsonObject->SetBoolField(TEXT("stream"), bEnableStreaming);
    
    FString RequestContent;
    const TSharedRef<TJsonWriter<>> RequestJsonWriter = TJsonStringWriter<>::Create(&RequestContent);
    FJsonSerializer::Serialize(RequestJsonObject.ToSharedRef(), RequestJsonWriter);
	
	const TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	const FString Url = FString::Printf(TEXT("http://localhost:%s/v1/chat/completions"), *GetLlamaServerPort());
	
	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    HttpRequest->SetContentAsString(RequestContent);
    
	const TSharedPtr<bool> bAtLeastOneTokenWasGenerated = MakeShared<bool>(false);
	const TSharedPtr<int32> ReadContentLength = MakeShared<int32>(0);
	
    HttpRequest->OnRequestProgress64().BindLambda([InTokenGeneratedFunction, bAtLeastOneTokenWasGenerated, ReadContentLength](FHttpRequestPtr InHttpRequestPtr, uint64, uint64)
    {
        const FHttpResponsePtr HttpResponsePtr = InHttpRequestPtr->GetResponse();
    	
        if (HttpResponsePtr.IsValid())
        {
        	const FString FullResponseContent = HttpResponsePtr->GetContentAsString();
            
        	// Remove content that has already been read.
			const FString NewContent = FullResponseContent.RightChop(*ReadContentLength);
			*ReadContentLength = FullResponseContent.Len();

			if (!NewContent.IsEmpty())
			{
				TArray<FString> ResponseLines;
				NewContent.ParseIntoArrayLines(ResponseLines);
    
				for (const FString& ResponseLine : ResponseLines)
				{
					const FString DataString = TEXT("data: ");
					
					if (ResponseLine.StartsWith(DataString) && !ResponseLine.Contains(TEXT("[DONE]")))
					{
						// remove "data: "
						FString JsonResponseLine = ResponseLine.RightChop(DataString.Len());
                	
						TSharedPtr<FJsonObject> ResponseJsonObject;
						TSharedRef<TJsonReader<>> ResponseJsonReader = TJsonReaderFactory<>::Create(JsonResponseLine);
                	
						if (FJsonSerializer::Deserialize(ResponseJsonReader, ResponseJsonObject))
						{
							const TArray<TSharedPtr<FJsonValue>>* ChoiceJsonValues;
                    	
							if (ResponseJsonObject->TryGetArrayField(TEXT("choices"), ChoiceJsonValues) && ChoiceJsonValues && !ChoiceJsonValues->IsEmpty())
							{
								const TSharedPtr<FJsonObject> ChoiceJsonObject = (*ChoiceJsonValues)[0]->AsObject();
								const TSharedPtr<FJsonObject>* DeltaJsonObject;
								
								if (ChoiceJsonObject->TryGetObjectField(TEXT("delta"), DeltaJsonObject) && DeltaJsonObject)
								{
									FString GeneratedToken;
                        	
									if ((*DeltaJsonObject)->TryGetStringField(TEXT("content"), GeneratedToken))
									{
										InTokenGeneratedFunction(true, GeneratedToken);
										*bAtLeastOneTokenWasGenerated = true;
									}
								}
							}
						}
					}
				}
			}
        }
    });
	
	HttpRequest->OnProcessRequestComplete().BindLambda([InTokenGeneratedFunction, bAtLeastOneTokenWasGenerated, bEnableStreaming](FHttpRequestPtr, FHttpResponsePtr InHttpResponsePtr, const bool bInProcessedSuccessfully)
	{
		if (bInProcessedSuccessfully && InHttpResponsePtr.IsValid())
		{
			if (bEnableStreaming)
			{
				if (!*bAtLeastOneTokenWasGenerated)
				{
					InTokenGeneratedFunction(false, FString());
				}
				
				return;
			}
			
			const FString ResponseContent = InHttpResponsePtr->GetContentAsString();
			TSharedPtr<FJsonObject> ResponseJsonObject;
			const TSharedRef<TJsonReader<>> ResponseJsonReader = TJsonReaderFactory<>::Create(ResponseContent);

			if (FJsonSerializer::Deserialize(ResponseJsonReader, ResponseJsonObject))
			{
				const TArray<TSharedPtr<FJsonValue>>* ChoiceJsonValues;
				
				if (ResponseJsonObject->TryGetArrayField(TEXT("choices"), ChoiceJsonValues) && ChoiceJsonValues && !ChoiceJsonValues->IsEmpty())
				{
					const TSharedPtr<FJsonObject> ChoiceJsonObject = (*ChoiceJsonValues)[0]->AsObject();
					const TSharedPtr<FJsonObject>* MessageJsonObject ;
					
					if (ChoiceJsonObject->TryGetObjectField(TEXT("message"), MessageJsonObject) && MessageJsonObject)
					{
						FString GeneratedToken;

						if ((*MessageJsonObject)->TryGetStringField(TEXT("content"), GeneratedToken))
						{
							InTokenGeneratedFunction(true, GeneratedToken);
							return;
						}
					}
				}
			}
		}
		
		InTokenGeneratedFunction(false, FString());
	});
    
    HttpRequest->ProcessRequest();
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
				const FString Argument = LlamaServerOption->GetArgument().IsEmpty() ? FString() : FString::Printf(TEXT(" %s"), *LlamaServerOption->GetArgument());
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
