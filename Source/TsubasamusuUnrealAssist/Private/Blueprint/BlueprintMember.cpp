// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "BlueprintMember.h"
#include "Type/TsubasamusuUnrealAssistStructs.h"

void FBlueprintMember::AddReferencedObjects(FReferenceCollector& InReferenceCollector)
{
	InReferenceCollector.AddReferencedObject(OwnerBlueprint);
	InReferenceCollector.AddReferencedObjects(MemberReferencerBlueprints);
	InReferenceCollector.AddReferencedObjects(ReferencerBlueprints);
}

void FBlueprintMember::Initialize()
{
	// Check Member Referencer Blueprints
	{
		MemberReferencerBlueprints = ReferencerBlueprints;
		MemberReferencerBlueprints.RemoveAll([this](const TObjectPtr<const UBlueprint> InMemberReferencerBlueprint)
		{
			return !IsMemberReferencerBlueprint(InMemberReferencerBlueprint);
		});
		
		bCheckedMemberReferencerBlueprints = true;
	}
	
	AccessSpecifierOptimizationRow = MakeShared<FAccessSpecifierOptimizationRow>(GetMemberName(), GetCurrentAccessSpecifier(), GetOptimalAccessSpecifier());
}

TsubasamusuUnrealAssist::EAccessSpecifier FBlueprintMember::GetOptimalAccessSpecifier() const
{
	if (!CheckedMemberReferencerBlueprints())
	{
		return TsubasamusuUnrealAssist::EAccessSpecifier::None;
	}
	
	if (MemberReferencerBlueprints.IsEmpty())
	{
		return TsubasamusuUnrealAssist::EAccessSpecifier::Private;
	}
	
	for (const TObjectPtr<const UBlueprint> MemberReferencerBlueprint : MemberReferencerBlueprints)
	{
		const UClass* MemberReferencerClass = MemberReferencerBlueprint->GeneratedClass;
		const UClass* OwnerClass = OwnerBlueprint->GeneratedClass;
	
		if (!MemberReferencerClass->IsChildOf(OwnerClass))
		{
			return TsubasamusuUnrealAssist::EAccessSpecifier::Public;
		}
	}
	
	return TsubasamusuUnrealAssist::EAccessSpecifier::Protected;
}
