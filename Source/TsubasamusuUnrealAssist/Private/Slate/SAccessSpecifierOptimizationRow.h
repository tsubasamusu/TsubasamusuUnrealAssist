// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

struct FAccessSpecifierOptimizationRow;

class SAccessSpecifierOptimizationRow final : public SMultiColumnTableRow<TSharedPtr<FAccessSpecifierOptimizationRow>>
{
public:
    SLATE_BEGIN_ARGS(SAccessSpecifierOptimizationRow) {}
	    SLATE_ARGUMENT(TSharedPtr<FAccessSpecifierOptimizationRow>, RowItem)
	    SLATE_ARGUMENT(FName, CheckBoxColumnId)
	    SLATE_ARGUMENT(FName, MemberNameColumnId)
	    SLATE_ARGUMENT(FName, CurrentAccessSpecifierColumnId)
	    SLATE_ARGUMENT(FName, OptimalAccessSpecifierColumnId)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);
    
    //~ Begin SMultiColumnTableRow Interface
    virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& InColumnName) override;
	//~ End SMultiColumnTableRow Interface

	bool IsChecked() const;
	ECheckBoxState GetCheckedState() const;
	void ToggleCheckedState() const;
	
private:
    TSharedPtr<FAccessSpecifierOptimizationRow> RowItem;
	TSharedPtr<SCheckBox> CheckBox;
	
	FName CheckBoxColumnId;
	FName MemberNameColumnId;
	FName CurrentAccessSpecifierColumnId;
	FName OptimalAccessSpecifierColumnId;
};
