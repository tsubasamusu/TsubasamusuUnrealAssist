// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystemBase.h"
#include "Type/TsubasamusuUnrealAssistStructs.h"
#include "LlmManager.generated.h"

using FOnTokenGenerated = TFunction<void(const bool /*bInSucceeded*/, const FString& /*InGeneratedToken*/)>;

UCLASS()
class ULlmManager final : public UEditorSubsystem, public TEditorSubsystemBase<ULlmManager>
{
	GENERATED_BODY()
	
public:
	//~ Begin USubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& InSubsystemCollectionBase) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface
	
	void RestartLlamaServer();
	void GenerateToken(const FString& InPrompt, const FOnTokenGenerated& InTokenGeneratedFunction, const bool bEnableStreaming = true) const;
	
	FORCEINLINE FLlamaServerSettings GetLastAppliedLlamaServerSettings() const
	{
		return LastAppliedLlamaServerSettings;
	}
	
	FORCEINLINE ELlamaServerStatus GetLlamaServerStatus() const
	{
		return LlamaServerStatus;
	}
	
private:
	void StartLlamaServer();
	void StopLlamaServer();
	
	FString GetLlamaServerPort() const;
	
	FProcHandle LlamaServerProcessHandle;
	FLlamaServerSettings LastAppliedLlamaServerSettings;
	ELlamaServerStatus LlamaServerStatus;
};
