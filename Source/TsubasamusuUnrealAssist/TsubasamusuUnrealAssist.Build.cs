// Copyright (c) 2025, tsubasamusu All rights reserved.

using UnrealBuildTool;

public class TsubasamusuUnrealAssist : ModuleRules
{
	public TsubasamusuUnrealAssist(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] { });

		PrivateIncludePaths.AddRange(new string[] { });

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Projects",
            "InputCore",
            "EditorFramework",
            "UnrealEd",
            "ToolMenus",
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore"
        });

        DynamicallyLoadedModuleNames.AddRange(new string[] { });
	}
}