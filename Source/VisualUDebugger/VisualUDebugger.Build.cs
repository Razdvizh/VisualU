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
				"VisualU",
                "Core",
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
			}
			);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"GameplayDebugger"
			}
			);

		SetupGameplayDebuggerSupport(Target);
	}
}
