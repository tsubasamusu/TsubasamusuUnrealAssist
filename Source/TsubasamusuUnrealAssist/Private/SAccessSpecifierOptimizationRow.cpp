// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "SAccessSpecifierOptimizationRow.h"
#include "TsubasamusuBlueprintEditor.h"

#define LOCTEXT_NAMESPACE "TsubasamusuUnrealAssist"

void SAccessSpecifierOptimizationRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	RowItem = InArgs._RowItem;
	CheckBoxColumnId = InArgs._CheckBoxColumnId;
	MemberNameColumnId = InArgs._MemberNameColumnId;
	CurrentAccessSpecifierColumnId = InArgs._CurrentAccessSpecifierColumnId;
	RecommendedAccessSpecifierColumnId = InArgs._RecommendedAccessSpecifierColumnId;
	
	SMultiColumnTableRow<TSharedPtr<FAccessSpecifierOptimizationRow>>::Construct(FSuperRowType::FArguments(), InOwnerTableView);
}

TSharedRef<SWidget> SAccessSpecifierOptimizationRow::GenerateWidgetForColumn(const FName& InColumnName)
{
	TSharedPtr<SWidget> ColumnContent;
	
	if (InColumnName == CheckBoxColumnId)
	{
		RowItem->CheckBox = SNew(SCheckBox)
			.IsChecked(ECheckBoxState::Checked)
			.OnCheckStateChanged_Lambda([this](const ECheckBoxState NewState)
			{
				RowItem->bSelected = NewState == ECheckBoxState::Checked;
			});
		
		ColumnContent = RowItem->CheckBox;
	}
	else if (InColumnName == MemberNameColumnId)
	{
		ColumnContent = SNew(STextBlock)
			.Text(FText::FromName(RowItem->MemberName));
	}
	else
	{
		const TsubasamusuBlueprintEditor::EAccessSpecifier AccessSpecifier = InColumnName == CurrentAccessSpecifierColumnId ? RowItem->CurrentAccessSpecifier : RowItem->RecommendedAccessSpecifier;
		FText AccessSpecifierText;
		
		switch (AccessSpecifier)
		{
		case TsubasamusuBlueprintEditor::EAccessSpecifier::None:
			AccessSpecifierText = LOCTEXT("AccessSpecifier_None", "None");
			break;
		case TsubasamusuBlueprintEditor::EAccessSpecifier::Private:
			AccessSpecifierText = LOCTEXT("AccessSpecifier_Private", "Private");
			break;
		case TsubasamusuBlueprintEditor::EAccessSpecifier::Protected:
			AccessSpecifierText = LOCTEXT("AccessSpecifier_Protected", "Protected");
			break;
		case TsubasamusuBlueprintEditor::EAccessSpecifier::Public:
			AccessSpecifierText = LOCTEXT("AccessSpecifier_Public", "Public");
			break;
		default:
			checkNoEntry();
		}
		
		ColumnContent = SNew(STextBlock)
			.Text(AccessSpecifierText)
			.ColorAndOpacity(InColumnName == CurrentAccessSpecifierColumnId ? FLinearColor::Red : FLinearColor::Green);
	}
	
	return SNew(SBox)
		.HAlign(HAlign_Center)
		.Padding(5.f, 0.f)
		[
			ColumnContent.ToSharedRef()
		];
}

#undef LOCTEXT_NAMESPACE
