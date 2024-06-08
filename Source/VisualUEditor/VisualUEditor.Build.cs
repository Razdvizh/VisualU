// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VisualUEditor : ModuleRules
{
	public VisualUEditor(ReadOnlyTargetRules Target) : base(Target)
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
				"PropertyEditor"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
	}
}
