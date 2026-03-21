// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "TickableEditorSubsystem.generated.h"

class UTsubasamusuUnrealAssistSettings;

UCLASS(Abstract)
class UTickableEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
	
public:
	//~ Begin USubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& InSubsystemCollectionBase) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface
	
protected:
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
