// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

class FTsubasamusuBlueprintEditorCommands  final : public TCommands<FTsubasamusuBlueprintEditorCommands>
{
public:
	FTsubasamusuBlueprintEditorCommands()
		: TCommands<FTsubasamusuBlueprintEditorCommands>(TEXT("TsubasamusuBlueprintEditor"), NSLOCTEXT("Contexts", "TsubasamusuBlueprintEditor", "Tsubasamusu Blueprint Editor"), NAME_None, FAppStyle::GetAppStyleSetName())
	{
	}	

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> OptimizeAccessSpecifiers;
};
