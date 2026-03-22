// Copyright (c) 2026, tsubasamusu All rights reserved.

#include "SAccessSpecifierOptimizationRow.h"
#include "Type/TsubasamusuUnrealAssistEnums.h"
#include "Type/TsubasamusuUnrealAssistStructs.h"
#include "Type/TsubasamusuUnrealAssistMacros.h"

#define LOCTEXT_NAMESPACE "SAccessSpecifierOptimizationRow"

void SAccessSpecifierOptimizationRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	RowItem = InArgs._RowItem;
	CheckBoxColumnId = InArgs._CheckBoxColumnId;
	MemberNameColumnId = InArgs._MemberNameColumnId;
	CurrentAccessSpecifierColumnId = InArgs._CurrentAccessSpecifierColumnId;
	OptimalAccessSpecifierColumnId = InArgs._OptimalAccessSpecifierColumnId;
	
	SMultiColumnTableRow<TSharedPtr<FAccessSpecifierOptimizationRow>>::Construct(FSuperRowType::FArguments(), InOwnerTableView);
}

TSharedRef<SWidget> SAccessSpecifierOptimizationRow::GenerateWidgetForColumn(const FName& InColumnName)
{
	TSharedPtr<SWidget> ColumnContent;
	
	if (InColumnName == CheckBoxColumnId)
	{
		ColumnContent = SAssignNew(CheckBox, SCheckBox)
			.IsChecked(ECheckBoxState::Checked);
	}
	else if (InColumnName == MemberNameColumnId)
	{
		ColumnContent = SNew(STextBlock)
			.Text(FText::FromName(RowItem->MemberName));
	}
	else
	{
		const ETsubasamusuAccessSpecifier AccessSpecifier = InColumnName == CurrentAccessSpecifierColumnId ? RowItem->CurrentAccessSpecifier : RowItem->OptimalAccessSpecifier;
		FText AccessSpecifierText;
		
		switch (AccessSpecifier)
		{
		case ETsubasamusuAccessSpecifier::Private:
			AccessSpecifierText = LOCTEXT("AccessSpecifier_Private", "Private");
			break;
		case ETsubasamusuAccessSpecifier::Protected:
			AccessSpecifierText = LOCTEXT("AccessSpecifier_Protected", "Protected");
			break;
		case ETsubasamusuAccessSpecifier::Public:
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
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 1, 0)
		.Padding(5.f, 0.f)
#else
		.Padding(FMargin(5.f, 0.f))
#endif
		[
			ColumnContent.ToSharedRef()
		];
}

bool SAccessSpecifierOptimizationRow::IsChecked() const
{
	return GetCheckedState() == ECheckBoxState::Checked;
}

ECheckBoxState SAccessSpecifierOptimizationRow::GetCheckedState() const
{
	if (CheckBox.IsValid())
	{
		return CheckBox->GetCheckedState();
	}
	
	return ECheckBoxState::Undetermined;
}

void SAccessSpecifierOptimizationRow::ToggleCheckedState() const
{
	if (CheckBox.IsValid())
	{
		return CheckBox->ToggleCheckedState();
	}
}

#undef LOCTEXT_NAMESPACE
