// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "TsubasamusuStringUtility.h"

TSharedRef<FTSTicker::FDelegateHandle> FTsubasamusuStringUtility::PlayTextAnimation(const FString& InBaseText, const FOnAnimationTextChanged& InAnimationTextChangedFunction, const float InAnimationDeltaSeconds, const int32 InMaxDotsNumber)
{
	const TSharedPtr<int32> ElapsedFramesCount = MakeShared<int32>(0);
	
	auto UpdateAnimationText = [InBaseText, InAnimationTextChangedFunction, InMaxDotsNumber, ElapsedFramesCount](float)
	{
		FString DotsString = TEXT("");
		
		for (int32 i = 0; i < (*ElapsedFramesCount % (InMaxDotsNumber + 1)); ++i)
		{
			DotsString += TEXT(".");
		}
		
		const FString AnimationText = FString::Printf(TEXT("%s%s"), *InBaseText, *DotsString);
		InAnimationTextChangedFunction(AnimationText);
		
		++(*ElapsedFramesCount);
		
		return true;
	};
	
	TSharedRef<FTSTicker::FDelegateHandle> TickerHandle = MakeShared<FTSTicker::FDelegateHandle>();
	*TickerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda(UpdateAnimationText), InAnimationDeltaSeconds);
	
	return TickerHandle;
}
