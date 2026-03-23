// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "Type/TsubasamusuUnrealAssistMacros.h"
#include "Type/TsubasamusuUnrealAssistEnums.h"
#include "CoreMinimal.h"

struct FAccessSpecifierOptimizationRow;
class UK2Node_FunctionEntry;
class SAccessSpecifierOptimizationRow;

class FBlueprintMember
{
public:
	FBlueprintMember(const TWeakObjectPtr<UBlueprint> InOwnerBlueprint, const TArray<TWeakObjectPtr<const UBlueprint>>& InReferencerBlueprints)
		: OwnerBlueprint(InOwnerBlueprint), ReferencerBlueprints(InReferencerBlueprints){}
	
	virtual ~FBlueprintMember() = default;

	void Initialize();
	virtual ETsubasamusuAccessSpecifier GetCurrentAccessSpecifier() const = 0;
	virtual ETsubasamusuAccessSpecifier GetOptimalAccessSpecifier() const;
	virtual void SetAccessSpecifier(const ETsubasamusuAccessSpecifier InAccessSpecifier) = 0;
	virtual FName GetMemberName() const = 0;
	
	FORCEINLINE TSharedPtr<FAccessSpecifierOptimizationRow> GetAccessSpecifierOptimizationRow() const
	{
		return AccessSpecifierOptimizationRow;
	}
	
	TSharedPtr<SAccessSpecifierOptimizationRow> AccessSpecifierOptimizationRowWidget;
	
protected:
	virtual bool IsMemberReferencerBlueprint(const TWeakObjectPtr<const UBlueprint> InBlueprint) const = 0;
	
	FORCEINLINE bool CheckedMemberReferencerBlueprints() const
	{
		return bCheckedMemberReferencerBlueprints;
	}
	
	TWeakObjectPtr<UBlueprint> OwnerBlueprint;
	TArray<TWeakObjectPtr<const UBlueprint>> MemberReferencerBlueprints;
	
private:
	TArray<TWeakObjectPtr<const UBlueprint>> ReferencerBlueprints;
	TSharedPtr<FAccessSpecifierOptimizationRow> AccessSpecifierOptimizationRow;
	bool bCheckedMemberReferencerBlueprints = false;
};

class FBlueprintMember_Variable final : public FBlueprintMember
{
public:
	FBlueprintMember_Variable(const TWeakObjectPtr<UBlueprint>& InOwnerBlueprint, const TArray<TWeakObjectPtr<const UBlueprint>>& InReferencerBlueprints, FProperty* InVariable)
		: FBlueprintMember(InOwnerBlueprint, InReferencerBlueprints), Variable(InVariable){}

	//~ Begin FBlueprintMember Interface
	virtual ETsubasamusuAccessSpecifier GetCurrentAccessSpecifier() const override;
	virtual ETsubasamusuAccessSpecifier GetOptimalAccessSpecifier() const override;
	virtual void SetAccessSpecifier(const ETsubasamusuAccessSpecifier InAccessSpecifier) override;
	virtual FName GetMemberName() const override;
	//~ End FBlueprintMember Interface
	
protected:
	//~ Begin FBlueprintMember Interface
	virtual bool IsMemberReferencerBlueprint(const TWeakObjectPtr<const UBlueprint> InBlueprint) const override;
	//~ End FBlueprintMember Interface
	
private:
	FProperty* Variable;
};

class FBlueprintMember_FunctionBase : public FBlueprintMember
{
public:
	FBlueprintMember_FunctionBase(const TWeakObjectPtr<UBlueprint>& InOwnerBlueprint, const TArray<TWeakObjectPtr<const UBlueprint>>& InReferencerBlueprints, const TWeakObjectPtr<UFunction> InFunction, const TWeakObjectPtr<UK2Node_EditablePinBase> InEntryNode)
		: FBlueprintMember(InOwnerBlueprint, InReferencerBlueprints), Function(InFunction), EntryNode(InEntryNode){}

	//~ Begin FBlueprintMember Interface
	virtual ETsubasamusuAccessSpecifier GetCurrentAccessSpecifier() const override;
	virtual FName GetMemberName() const override;
	//~ End FBlueprintMember Interface
	
protected:
	//~ Begin FBlueprintMember Interface
	virtual bool IsMemberReferencerBlueprint(const TWeakObjectPtr<const UBlueprint> InBlueprint) const override;
	//~ End FBlueprintMember Interface
	
	template<typename EntryNodeType>
	FORCEINLINE TWeakObjectPtr<EntryNodeType> GetEntryNodeChecked() const
	{
		static_assert(TIsDerivedFrom<EntryNodeType, UK2Node_EditablePinBase>::Value, "EntryNodeType must be derived from UK2Node_EditablePinBase.");
		return CastChecked<EntryNodeType>(EntryNode);
	}
	
	TWeakObjectPtr<UFunction> Function;
	
private:
	TWeakObjectPtr<UK2Node_EditablePinBase> EntryNode;
};

class FBlueprintMember_Function final : public FBlueprintMember_FunctionBase
{
public:
	FBlueprintMember_Function(const TWeakObjectPtr<UBlueprint>& InOwnerBlueprint, const TArray<TWeakObjectPtr<const UBlueprint>>& InReferencerBlueprints, const TWeakObjectPtr<UFunction> InFunction, const TWeakObjectPtr<UK2Node_EditablePinBase> InEntryNode)
		: FBlueprintMember_FunctionBase(InOwnerBlueprint, InReferencerBlueprints, InFunction, InEntryNode){}

	//~ Begin FBlueprintMember Interface
	virtual void SetAccessSpecifier(const ETsubasamusuAccessSpecifier InAccessSpecifier) override;
	//~ End FBlueprintMember Interface
};

#if CUSTOM_EVENT_ACCESS_SPECIFIER_IS_SUPPORTED
class FBlueprintMember_CustomEvent final : public FBlueprintMember_FunctionBase
{
public:
	FBlueprintMember_CustomEvent(const TWeakObjectPtr<UBlueprint>& InOwnerBlueprint, const TArray<TWeakObjectPtr<const UBlueprint>>& InReferencerBlueprints, const TWeakObjectPtr<UFunction> InFunction, const TWeakObjectPtr<UK2Node_EditablePinBase> InEntryNode)
	: FBlueprintMember_FunctionBase(InOwnerBlueprint, InReferencerBlueprints, InFunction, InEntryNode){}

	//~ Begin FBlueprintMember Interface
	virtual void SetAccessSpecifier(const ETsubasamusuAccessSpecifier InAccessSpecifier) override;
	//~ End FBlueprintMember Interface
};
#endif
