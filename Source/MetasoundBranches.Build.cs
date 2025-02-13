// Copyright 2025 Charles Matthews. All Rights Reserved.

using UnrealBuildTool;

public class MetasoundBranches : ModuleRules
{
    public MetasoundBranches(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
       
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "MetasoundEngine",
                "MetasoundStandardNodes",
                "MetasoundFrontend", 
                "MetasoundGraphCore"
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