// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "EditorSubsystemBase.h"

void UEditorSubsystemBase::Initialize(FSubsystemCollectionBase& InSubsystemCollectionBase)
{
	PostEngineInitHandle = FCoreDelegates::OnPostEngineInit.AddUObject(this, &UEditorSubsystemBase::OnPostEngineInit);
}

void UEditorSubsystemBase::Deinitialize()
{
	if (PostEngineInitHandle.IsValid())
	{
		FCoreDelegates::OnPostEngineInit.Remove(PostEngineInitHandle);
	}
	
	if (AssetEditorOpenedHandle.IsValid() && IsValid(GEditor))
	{
		UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
		AssetEditorSubsystem->OnAssetEditorOpened().Remove(AssetEditorOpenedHandle);
	}
}

void UEditorSubsystemBase::OnPostEngineInit()
{
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	AssetEditorOpenedHandle = AssetEditorSubsystem->OnAssetEditorOpened().AddUObject(this, &UEditorSubsystemBase::OnAssetEditorOpened);
}
