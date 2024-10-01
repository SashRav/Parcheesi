// Copyright Cats and Cubes. All Rights Reserved.

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class Parcheesi : ModuleRules
{
    public Parcheesi(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "Niagara",
            "UMG",
            "EnhancedInput" ,
            "OnlineSubsystem",
            "OnlineSubsystemUtils",
            "AIModule"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        PublicIncludePaths.AddRange(new string[] { });
    }
}
