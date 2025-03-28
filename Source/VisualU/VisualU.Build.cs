// Copyright (c) 2024 Evgeny Shustov

using UnrealBuildTool;

public class VisualU : ModuleRules
{
	public VisualU(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Paper2D",
                "Core",
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "UMG",
                "AssetRegistry",
				"GameplayDebugger",
                "MovieScene",
                "MovieSceneTracks"
				// ... add other public dependencies that you statically link with here ...
			}
			);

		if (Target.Type.Equals(TargetType.Editor))
		{
			PublicDependencyModuleNames.Add("Settings");
		}

		SetupGameplayDebuggerSupport(Target);

	}
}
