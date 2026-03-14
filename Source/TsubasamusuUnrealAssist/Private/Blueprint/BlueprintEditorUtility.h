// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SCheckBoxList.h"
#include "UnusedItem.h"
#include "Debug/EditorMessageUtility.h"

class FBlueprintEditor;
class FBlueprintCommandContext;

#define LOCTEXT_NAMESPACE "FBlueprintEditorUtility"

class FBlueprintEditorUtility final
{
public:
	static void RegisterCommandInBlueprintEditMenu(const FBlueprintCommandContext& InBlueprintCommandContext);
	static TSharedPtr<FBlueprintEditor> GetBlueprintEditor(const UBlueprint* InBlueprint);
	
	template<typename FunctionToDeleteUnusedItem>
	static void HandleDeletingUnusedItem(const int32 InCheckedItemsCount, const FText& InItemTypeText, const TArray<TSharedPtr<FUnusedItem>>& InUnusedItems, const FunctionToDeleteUnusedItem& InFunctionToDeleteUnusedItem)
	{
		if (InCheckedItemsCount == 0)
		{
			const FText NotificationText =  FText::Format(LOCTEXT("NoItemsToCheck", "No {0} to check for."), InItemTypeText);
			FEditorMessageUtility::DisplaySimpleNotification(NotificationText);
			return;
		}
		
		if (InUnusedItems.IsEmpty())
		{
			const FText NotificationText = FText::Format(LOCTEXT("NoUnusedItems", "There are no unused {0}."), InItemTypeText);
			FEditorMessageUtility::DisplaySimpleNotification(NotificationText);
			return;
		}
		
		const FString TitleCaseItemTypeString = ConvertToTitleCaseString(InItemTypeText.ToString());
		const FText TitleCaseItemType = FText::FromString(TitleCaseItemTypeString);
		
		const TSharedRef<SCheckBoxList> CheckBoxList = SNew(SCheckBoxList).ItemHeaderLabel(TitleCaseItemType);
		
		for (const TSharedPtr<FUnusedItem> UnusedItem : InUnusedItems)
		{
			CheckBoxList->AddItem(UnusedItem->GetItemNameText(), true);
		}
		
		const int32 UnusedItemsCount = InUnusedItems.Num();
		auto OneOrMoreItemsAreChecked = [&UnusedItemsCount, CheckBoxList]()
		{
			for (int32 Index = 0; Index < UnusedItemsCount; ++Index)
			{
				if (CheckBoxList->IsItemChecked(Index))
				{
					return true;
				}
			}
			
			return false;
		};
		
		const FText DialogTitle = FText::Format(LOCTEXT("DeleteUnusedItemsDialog_Title", "Delete Unused {0}"), TitleCaseItemType);
		const FText DialogMessage = FText::Format(LOCTEXT("DeleteUnusedItemsDialog_Message", "These {0} are not used."), InItemTypeText);
		const FText ApplyButtonText = FText::Format(LOCTEXT("DeleteUnusedItemsDialog_ApplyButton", "Delete Selected {0}"), TitleCaseItemType);
		const FText CancelButtonText = LOCTEXT("DeleteUnusedItemsDialog_CancelButton", "Cancel");
		
		const ETsubasamusuDialogButton PressedButton = FEditorMessageUtility::ShowCustomDialog(DialogTitle, DialogMessage, ApplyButtonText, CancelButtonText, CheckBoxList, TAttribute<bool>::CreateLambda(OneOrMoreItemsAreChecked));
		
		if (PressedButton != ETsubasamusuDialogButton::OK || !OneOrMoreItemsAreChecked())
		{
			return;
		}
		
		for (int32 Index = 0; Index < UnusedItemsCount; ++Index)
		{
			if (CheckBoxList->IsItemChecked(Index))
			{
				InFunctionToDeleteUnusedItem(Index);
			}
		}
		
		const FText NotificationText = FText::Format(LOCTEXT("SuccessfullyDeleteUnusedItems", "Successfully delete unused {0}."), InItemTypeText);
		FEditorMessageUtility::DisplaySimpleNotification(NotificationText, SNotificationItem::ECompletionState::CS_Success);
	}

private:
	static FString ConvertToTitleCaseString(const FString& InSourceString);
};

#undef LOCTEXT_NAMESPACE
