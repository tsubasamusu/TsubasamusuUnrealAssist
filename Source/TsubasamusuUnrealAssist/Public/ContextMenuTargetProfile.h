// Copyright (c) 2025, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintActionMenuUtils.h"

struct FContextMenuTargetProfile
{
public:
	FContextMenuTargetProfile();
	explicit FContextMenuTargetProfile(const FBlueprintActionContext& MenuContext);

	uint32 GetContextTargetMask() const;
	void   SetContextTarget(EContextTargetFlags::Type Flag, bool bClear = false);
	uint32 GetIncompatibleTargetsMask() const;

	bool   IsTargetEnabled(EContextTargetFlags::Type Flag) const;

private:
	void SaveProfile() const;
	bool LoadProfile();

	FString ProfileSaveName;
	uint32  HasComponentsMask;
	uint32  IncompatibleTargetFlags;
	uint32  SavedTargetFlags;
};
