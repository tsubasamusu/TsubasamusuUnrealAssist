// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "AssetTypeActions_TsubasamusuBlueprint.h"
#include "FMessageDialogUtility.h"
#include "TsubasamusuBlueprintEditor.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "TsubasamusuUnrealAssist"

void FAssetTypeActions_TsubasamusuBlueprint::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type ToolkitMode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (UObject* Object : InObjects)
	{
		UBlueprint* Blueprint = Cast<UBlueprint>(Object);
		
		if (IsValid(Blueprint))
		{
			bool bLetOpen = true;
			
			if (!Blueprint->SkeletonGeneratedClass || !Blueprint->GeneratedClass)
			{
				const FText WarningMessage = LOCTEXT("FailedToLoadTsubasamusuBlueprintWithContinue", "Blueprint could not be loaded because it derives from an invalid class.  Check to make sure the parent class for this blueprint hasn't been removed! Do you want to continue (it can crash the editor)?");
				bLetOpen = EAppReturnType::Yes == FMessageDialogUtility::OpenWarningMessageDialog(EAppMsgType::YesNo, WarningMessage);
			}
			
			if (bLetOpen)
			{
				const TSharedRef<FTsubasamusuBlueprintEditor> TsubasamusuBlueprintEditor(new FTsubasamusuBlueprintEditor());

				TArray<UBlueprint*> Blueprints
				{
					Blueprint
				};

				TsubasamusuBlueprintEditor->InitTsubasamusuBlueprintEditor(ToolkitMode, EditWithinLevelEditor, Blueprints, ShouldUseDataOnlyEditor(Blueprint));
			}
		}
		else
		{
			const FText WarningMessage = LOCTEXT("FailedToLoadTusbasamusuBlueprint", "Blueprint could not be loaded because it derives from an invalid class.  Check to make sure the parent class for this blueprint hasn't been removed!");
			FMessageDialogUtility::OpenWarningMessageDialog(EAppMsgType::Ok, WarningMessage);
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
