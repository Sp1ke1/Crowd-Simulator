// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class CrowdSimulator : ModuleRules
{
	public CrowdSimulator(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "AIModule", "UMG", "NavigationSystem", "DeveloperSettings"
		});

		/*PublicIncludePaths.Add(Path.Combine(EngineDirectory, "Source/Runtime/AIModule")); */
	}
}
