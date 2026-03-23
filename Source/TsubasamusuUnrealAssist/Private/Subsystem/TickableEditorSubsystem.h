// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystemBase.h"
#include "TickableEditorSubsystem.generated.h"

class UTsubasamusuUnrealAssistSettings;

UCLASS(Abstract)
class UTickableEditorSubsystem : public UEditorSubsystemBase
{
	GENERATED_BODY()
	
public:
	//~ Begin USubsystem Interface
	virtual void Deinitialize() override;
	//~ End USubsystem Interface
	
protected:
	//~ Begin UEditorSubsystemBase Interface
	virtual void OnPostEngineInit() override;
	//~ End UEditorSubsystemBase Interface
	
	virtual void Tick(const float InDeltaTime){}
	
	FORCEINLINE TWeakObjectPtr<UTsubasamusuUnrealAssistSettings> GetCachedTsubasamusuUnrealAssistSettings() const
	{
		return CachedTsubasamusuUnrealAssistSettings;
	}
	
private:
	void RegisterTicker();
	void UnregisterTicker() const;
	
	FTSTicker::FDelegateHandle TickerHandle;
	TWeakObjectPtr<UTsubasamusuUnrealAssistSettings> CachedTsubasamusuUnrealAssistSettings;
};
