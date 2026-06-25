using UnrealBuildTool;

public class ThreeDTextPrompt : ModuleRules
{
    public ThreeDTextPrompt(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "Text3D",
            "DeveloperSettings"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
