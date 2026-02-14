// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

#include "CoreMinimal.h"

namespace TsubasamusuBlueprintEditor
{
	enum class EAccessSpecifier : uint8;
}

struct FAccessSpecifierOptimizationRow;

class SAccessSpecifierOptimizationRow final : public SMultiColumnTableRow<TSharedPtr<FAccessSpecifierOptimizationRow>>
{
public:
    SLATE_BEGIN_ARGS(SAccessSpecifierOptimizationRow) {}
	    SLATE_ARGUMENT(TSharedPtr<FAccessSpecifierOptimizationRow>, RowItem)
	    SLATE_ARGUMENT(FName, CheckboxColumnId)
	    SLATE_ARGUMENT(FName, MemberNameColumnId)
	    SLATE_ARGUMENT(FName, CurrentAccessSpecifierColumnId)
	    SLATE_ARGUMENT(FName, RecommendedAccessSpecifierColumnId)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);
    
    //~ Begin SMultiColumnTableRow Interface
    virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& InColumnName) override;
	//~ End SMultiColumnTableRow Interface

private:
    TSharedPtr<FAccessSpecifierOptimizationRow> RowItem;
	
	FName CheckboxColumnId;
	FName MemberNameColumnId;
	FName CurrentAccessSpecifierColumnId;
	FName RecommendedAccessSpecifierColumnId;
};
