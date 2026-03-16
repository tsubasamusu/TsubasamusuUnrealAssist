// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystemBase.h"
#include "LlmManager.generated.h"

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
	
private:
	void StartLlamaServer();
	void StopLlamaServer();
	
	FProcHandle LlamaServerProcessHandle;
};
