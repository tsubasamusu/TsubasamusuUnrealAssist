// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "EditorSubsystemBase.h"
#include "Type/TsubasamusuUnrealAssistMacros.h"

void UEditorSubsystemBase::Initialize(FSubsystemCollectionBase& InSubsystemCollectionBase)
{
	FSimpleMulticastDelegate& PostEngineInitDelegate =
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 8, 0)
		FCoreDelegates::GetOnPostEngineInit();
#else
		FCoreDelegates::OnPostEngineInit;
#endif
	
	PostEngineInitHandle = PostEngineInitDelegate.AddUObject(this, &UEditorSubsystemBase::OnPostEngineInit);
}

void UEditorSubsystemBase::Deinitialize()
{
	if (PostEngineInitHandle.IsValid())
	{
		FSimpleMulticastDelegate& PostEngineInitDelegate =
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 8, 0)
			FCoreDelegates::GetOnPostEngineInit();
#else
			FCoreDelegates::OnPostEngineInit;
#endif
		
		PostEngineInitDelegate.Remove(PostEngineInitHandle);
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
