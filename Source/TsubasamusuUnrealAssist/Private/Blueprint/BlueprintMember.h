// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "Type/TsubasamusuUnrealAssistEnums.h"
#include "CoreMinimal.h"

struct FAccessSpecifierOptimizationRow;
class SAccessSpecifierOptimizationRow;

class FBlueprintMember : public FGCObject
{
public:
	FBlueprintMember(const TObjectPtr<UBlueprint> InOwnerBlueprint, const TArray<TObjectPtr<const UBlueprint>>& InReferencerBlueprints)
		: OwnerBlueprint(InOwnerBlueprint), ReferencerBlueprints(InReferencerBlueprints){}
	
	//~ Begin FGCObject Interface
	virtual void AddReferencedObjects(FReferenceCollector& InReferenceCollector) override;
	virtual FString GetReferencerName() const override
	{
		return TEXT("FBlueprintMember");
	}
	//~ End FGCObject Interface
	
	void Initialize();
	virtual TsubasamusuUnrealAssist::EAccessSpecifier GetCurrentAccessSpecifier() const = 0;
	virtual TsubasamusuUnrealAssist::EAccessSpecifier GetOptimalAccessSpecifier() const;
	virtual void SetAccessSpecifier(const TsubasamusuUnrealAssist::EAccessSpecifier InAccessSpecifier) = 0;
	virtual FName GetMemberName() const = 0;
	
	FORCEINLINE TSharedPtr<FAccessSpecifierOptimizationRow> GetAccessSpecifierOptimizationRow() const
	{
		return AccessSpecifierOptimizationRow;
	}
	
	TSharedPtr<SAccessSpecifierOptimizationRow> AccessSpecifierOptimizationRowWidget;
	
protected:
	virtual bool IsMemberReferencerBlueprint(const UBlueprint* InBlueprint) const = 0;
	
	FORCEINLINE bool CheckedMemberReferencerBlueprints() const
	{
		return bCheckedMemberReferencerBlueprints;
	}
	
	TObjectPtr<UBlueprint> OwnerBlueprint;
	TArray<TObjectPtr<const UBlueprint>> MemberReferencerBlueprints;
	
private:
	TArray<TObjectPtr<const UBlueprint>> ReferencerBlueprints;
	TSharedPtr<FAccessSpecifierOptimizationRow> AccessSpecifierOptimizationRow;
	bool bCheckedMemberReferencerBlueprints = false;
};
