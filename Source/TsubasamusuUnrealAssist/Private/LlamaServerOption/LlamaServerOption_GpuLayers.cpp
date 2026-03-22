// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "LlamaServerOption_GpuLayers.h"
#include "Type/TsubasamusuUnrealAssistMacros.h"

void ULlamaServerOption_GpuLayers::PostEditChangeProperty(FPropertyChangedEvent& InPropertyChangedEvent)
{
	Super::PostEditChangeProperty(InPropertyChangedEvent);
	
	const FName ChangedPropertyName = 
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 1, 0)
		InPropertyChangedEvent.GetMemberPropertyName();
#else
		InPropertyChangedEvent.MemberProperty ? InPropertyChangedEvent.MemberProperty->GetFName() : NAME_None;
#endif
	
	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(ULlamaServerOption_GpuLayers, GpuLayers))
	{
		switch (GpuLayers)
		{
		case EGpuLayers::Auto:
			GpuLayersNumber = -2;
			break;
		case EGpuLayers::All:
			GpuLayersNumber = -1;
			break;
		case EGpuLayers::ExactNumber:
			GpuLayersNumber = 0;
			break;
		default:
			checkNoEntry();
		}
	}
}

FString ULlamaServerOption_GpuLayers::GetParameter() const
{
	return TEXT("n-gpu-layers");
}

void ULlamaServerOption_GpuLayers::SetArgument(const FString& InArgument)
{
	GpuLayersNumber = FCString::Atoi(*InArgument);

	switch (GpuLayersNumber)
	{
	case -2:
		GpuLayers = EGpuLayers::Auto;
		break;
	case -1:
		GpuLayers = EGpuLayers::All;
		break;
	default:
		GpuLayers = EGpuLayers::ExactNumber;
	}
}

FString ULlamaServerOption_GpuLayers::GetArgument(const bool bForConfig) const
{
	switch (GpuLayers)
	{
	case EGpuLayers::Auto:
		return bForConfig ? FString::FromInt(-2) : TEXT("auto");
	case EGpuLayers::All:
		return bForConfig ? FString::FromInt(-1) : TEXT("all");
	case EGpuLayers::ExactNumber:
		return FString::FromInt(GpuLayersNumber);
	default:
		checkNoEntry();
		return FString();
	}
}

bool ULlamaServerOption_GpuLayers::IsValidArgument() const
{
	if (GpuLayers == EGpuLayers::ExactNumber)
	{
		return GpuLayersNumber >= 0;
	}
	
	return true;
}
