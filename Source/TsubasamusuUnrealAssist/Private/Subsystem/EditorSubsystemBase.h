// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystemBase.generated.h"

UCLASS(Abstract)
class UEditorSubsystemBase : public UEditorSubsystem
{
	GENERATED_BODY()
	
public:
	//~ Begin USubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& InSubsystemCollectionBase) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface
	
protected:
	virtual void OnPostEngineInit();
	virtual void OnAssetEditorOpened(UObject* InOpenedAsset){}
	
private:
	FDelegateHandle PostEngineInitHandle;
	FDelegateHandle AssetEditorOpenedHandle;
};

template<typename DerivedClass>
class TEditorSubsystemBase
{
public:
	static DerivedClass* GetChecked()
	{
		DerivedClass* EditorSubsystem = GEditor->GetEditorSubsystem<DerivedClass>();
		check(IsValid(EditorSubsystem));
		return EditorSubsystem;
	}
};
