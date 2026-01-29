// Copyright (c) 2026, tsubasamusu All rights reserved.

using UnrealBuildTool;

public class TsubasamusuUnrealAssist : ModuleRules
{
	public TsubasamusuUnrealAssist(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"UnrealEd",
			"CoreUObject",
			"Engine",
			"Blutility",
			"GraphEditor",
			"InternationalizationSettings",
            "BlueprintGraph",
            "OutputLog",
            "Slate",
            "SlateCore",
            "InputCore",
            "HTTP",
            "Json",
            "JsonUtilities",
            "AssetTools",
            "Kismet",
            "SubobjectEditor",
            "ApplicationCore",
            "ToolMenus",
            "ContentBrowserData",
            "ContentBrowserAssetDataSource",
            "AssetRegistry"
		});
	}
}