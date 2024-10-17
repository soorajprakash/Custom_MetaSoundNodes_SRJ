#include "MetasoundBranches/Public/MetasoundBranches.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FMetasoundBranchesModule, MetasoundBranches)

void FMetasoundBranchesModule::StartupModule()
{

    UE_LOG(LogTemp, Log, TEXT("MetasoundBranches module started..."));
    
    // Initialization
}

void FMetasoundBranchesModule::ShutdownModule()
{

    UE_LOG(LogTemp, Log, TEXT("MetasoundBranches module shutting down..."));

    // Cleanup
}