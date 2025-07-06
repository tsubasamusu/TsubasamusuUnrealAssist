// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "BlueprintEditor/AssetTypeActions_TsubasamusuBlueprint.h"
#include "BlueprintEditor/TsubasamusuBlueprintEditor.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "TsubasamusuAssetTypeActions"

void FAssetTypeActions_TsubasamusuBlueprint::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjectConstIterator = InObjects.CreateConstIterator(); ObjectConstIterator; ++ObjectConstIterator)
	{
		auto Blueprint = Cast<UBlueprint>(*ObjectConstIterator);

		if (!IsValid(Blueprint))
		{
			const FText Message = LOCTEXT("FailedToLoadTsubasamusuBlueprint", "Tsubasamusu Blueprint could not be loaded because it derives from an invalid class.  Check to make sure the parent class for this blueprint hasn't been removed!");
			FMessageDialog::Open(EAppMsgType::Ok, Message);
			
			continue;
		}
		
		bool bLetOpen = true;
		
		if (!IsValid(Blueprint->ParentClass))
		{
			const FText Message = LOCTEXT("FailedToLoadTsubasamusuBlueprintWithContinue", "Tsubasamusu Blueprint could not be loaded because it derives from an invalid class.  Check to make sure the parent class for this blueprint hasn't been removed! Do you want to continue (it can crash the editor)?");
			bLetOpen = EAppReturnType::Yes == FMessageDialog::Open(EAppMsgType::YesNo, Message);
		}
		
		if (bLetOpen)
		{
			const TSharedRef<FTsubasamusuBlueprintEditor> NewEditor(new FTsubasamusuBlueprintEditor());

			TArray<UBlueprint*> Blueprints;
			Blueprints.Add(Blueprint);

			NewEditor->InitTsubasamusuBlueprintEditor(Mode, EditWithinLevelEditor, Blueprints, ShouldUseDataOnlyEditor(Blueprint));
		}
	}
}

bool FAssetTypeActions_TsubasamusuBlueprint::ShouldUseDataOnlyEditor(const UBlueprint* Blueprint)
{
	return FBlueprintEditorUtils::IsDataOnlyBlueprint(Blueprint) 
		&& !FBlueprintEditorUtils::IsLevelScriptBlueprint(Blueprint) 
		&& !FBlueprintEditorUtils::IsInterfaceBlueprint(Blueprint)
		&& !Blueprint->bForceFullEditor
		&& !Blueprint->bIsNewlyCreated;
}

#undef LOCTEXT_NAMESPACE
