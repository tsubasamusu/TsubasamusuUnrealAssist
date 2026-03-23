// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystemBase.h"
#include "Type/TsubasamusuUnrealAssistStructs.h"
#include "LlmManager.generated.h"

using FOnTokenGenerated = TFunction<void(const bool /*bInSucceeded*/, const FString& /*InGeneratedToken*/)>;
using FOnLlamaServerStatusChecked = TFunction<void(const ELlamaServerStatus /*InLlamaServerStatus*/)>;

UCLASS()
class ULlmManager final : public UEditorSubsystemBase, public TEditorSubsystemBase<ULlmManager>
{
	GENERATED_BODY()
	
public:
	//~ Begin USubsystem Interface
	virtual void Deinitialize() override;
	//~ End USubsystem Interface
	
	void RestartLlamaServer();
	void GenerateToken(const FString& InPrompt, const FOnTokenGenerated& InTokenGeneratedFunction, const bool bEnableStreaming = true) const;
	
	bool LlamaServerIsRunning() const;
	bool LlamaServerIsBadStatus() const;
	bool LlamaServerCanRestart() const;
	
	FORCEINLINE FLlamaServerSettings GetLastAppliedLlamaServerSettings() const
	{
		return LastAppliedLlamaServerSettings;
	}
	
	FORCEINLINE ELlamaServerStatus GetLlamaServerStatus() const
	{
		return LlamaServerStatus;
	}
	
protected:
	//~ Begin UEditorSubsystemBase Interface
	virtual void OnPostEngineInit() override;
	//~ End UEditorSubsystemBase Interface
	
private:
	void StartLlamaServer();
	void StopLlamaServer();
	
	FString GetLlamaServerPort() const;
	void CheckLlamaServerStatus(const FOnLlamaServerStatusChecked& InLlamaServerStatusCheckedFunction) const;
	
	FProcHandle LlamaServerProcessHandle;
	FLlamaServerSettings LastAppliedLlamaServerSettings;
	ELlamaServerStatus LlamaServerStatus;
};
