// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

using FOnAnimationTextChanged = TFunction<void(const FString& /*InAnimationText*/)>;

class FTsubasamusuStringUtility final
{
public:
	static TSharedRef<FTSTicker::FDelegateHandle> PlayTextAnimation(const FString& InBaseText, const FOnAnimationTextChanged& InAnimationTextChangedFunction, const float InAnimationDeltaSeconds = 0.2f, const int32 InMaxDotsNumber = 3);
};
