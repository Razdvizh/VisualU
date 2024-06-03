// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VisualU : ModuleRules
{
	public VisualU(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Paper2D",
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
				"UMG",
                "AssetRegistry",
				"MovieScene",
                "MovieSceneTracks"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		//PublicDefinitions.Add("WITH_GAMEPLAY_DEBUGGER=" + (Target.bUseGameplayDebugger ? 1 : 0));

	}
}
