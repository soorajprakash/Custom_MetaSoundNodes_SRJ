using UnrealBuildTool;

public class MetasoundBranches : ModuleRules
{
    public MetasoundBranches(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicIncludePaths.AddRange(
            new string[] {
                //"MetasoundBranches/Public"
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                "MetasoundBranches/Private",
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "MetasoundEngine",
                "MetasoundStandardNodes"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "AudioExtensions",
                "SignalProcessing"
            }
        );
    }
}