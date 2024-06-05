// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VisualUDebugger : ModuleRules
{
	public VisualUDebugger(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"VisualU"
				// ... add other public dependencies that you statically link with here ...
			}
			);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "CoreUObject",
                "Engine",
                "Slate",
				"SlateCore",
				"GameplayDebugger"
				// ... add private dependencies that you statically link with here ...	
			}
			);

		SetupGameplayDebuggerSupport(Target);
	}
}
