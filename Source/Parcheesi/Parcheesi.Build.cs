// Copyright Cats and Cubes. All Rights Reserved.

using UnrealBuildTool;

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
            "UMG"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        PublicIncludePaths.AddRange(new string[] { });
    }
}
