// Copyright Broken Rock Studios LLC. All Rights Reserved.

using UnrealBuildTool;

// Automation tests for GameStateSubsystem. Developer module: not built for Shipping, so CQTest never leaks into a shipped target.
public class GameStateSubsystemTests : ModuleRules
{
	public GameStateSubsystemTests(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"CQTest",
				"ModularGameplayActors",
				"GameStateSubsystem",
			}
		);
	}
}
