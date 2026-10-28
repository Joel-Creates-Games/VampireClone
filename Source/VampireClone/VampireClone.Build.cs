// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VampireClone : ModuleRules
{
	public VampireClone(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"NavigationSystem",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"Niagara",
			"UMG",
			"Slate",
            "GameplayTags"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"VampireClone",
			"VampireClone/Variant_Strategy",
			"VampireClone/Variant_Strategy/UI",
			"VampireClone/Variant_TwinStick",
			"VampireClone/Variant_TwinStick/AI",
			"VampireClone/Variant_TwinStick/Gameplay",
			"VampireClone/Variant_TwinStick/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
