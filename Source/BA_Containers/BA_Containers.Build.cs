// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BA_Containers : ModuleRules
{
	public BA_Containers(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
