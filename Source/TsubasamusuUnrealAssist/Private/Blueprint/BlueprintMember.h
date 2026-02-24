// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "Type/TsubasamusuUnrealAssistEnums.h"
#include "CoreMinimal.h"

struct FAccessSpecifierOptimizationRow;
class UK2Node_FunctionEntry;
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

class FBlueprintMember_Variable final : public FBlueprintMember
{
public:
	FBlueprintMember_Variable(const TObjectPtr<UBlueprint>& InOwnerBlueprint, const TArray<TObjectPtr<const UBlueprint>>& InReferencerBlueprints, FProperty* InVariable)
		: FBlueprintMember(InOwnerBlueprint, InReferencerBlueprints), Variable(InVariable){}

	//~ Begin FBlueprintMember Interface
	virtual TsubasamusuUnrealAssist::EAccessSpecifier GetCurrentAccessSpecifier() const override;
	virtual TsubasamusuUnrealAssist::EAccessSpecifier GetOptimalAccessSpecifier() const override;
	virtual void SetAccessSpecifier(const TsubasamusuUnrealAssist::EAccessSpecifier InAccessSpecifier) override;
	virtual FName GetMemberName() const override;
	//~ End FBlueprintMember Interface
	
protected:
	//~ Begin FBlueprintMember Interface
	virtual bool IsMemberReferencerBlueprint(const UBlueprint* InBlueprint) const override;
	//~ End FBlueprintMember Interface
	
private:
	FProperty* Variable;
};

class FBlueprintMember_Function final : public FBlueprintMember
{
public:
	FBlueprintMember_Function(const TObjectPtr<UBlueprint>& InOwnerBlueprint, const TArray<TObjectPtr<const UBlueprint>>& InReferencerBlueprints, const TObjectPtr<UFunction> InFunction, const TObjectPtr<UK2Node_FunctionEntry> InFunctionEntryNode)
		: FBlueprintMember(InOwnerBlueprint, InReferencerBlueprints), Function(InFunction), FunctionEntryNode(InFunctionEntryNode){}

	//~ Begin FGCObject Interface
	virtual void AddReferencedObjects(FReferenceCollector& InReferenceCollector) override;
	//~ End FGCObject Interface
	
	//~ Begin FBlueprintMember Interface
	virtual TsubasamusuUnrealAssist::EAccessSpecifier GetCurrentAccessSpecifier() const override;
	virtual void SetAccessSpecifier(const TsubasamusuUnrealAssist::EAccessSpecifier InAccessSpecifier) override;
	virtual FName GetMemberName() const override;
	//~ End FBlueprintMember Interface
	
protected:
	//~ Begin FBlueprintMember Interface
	virtual bool IsMemberReferencerBlueprint(const UBlueprint* InBlueprint) const override;
	//~ End FBlueprintMember Interface
	
private:
	TObjectPtr<UFunction> Function;
	TObjectPtr<UK2Node_FunctionEntry> FunctionEntryNode;
};
