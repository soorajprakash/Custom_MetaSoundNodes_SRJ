#include "MetasoundBranches/Public/MetasoundBranches.h"
#include "MetasoundFrontendRegistries.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FMetasoundBranchesModule"

void FMetasoundBranchesModule::StartupModule()
{

    UE_LOG(LogTemp, Log, TEXT("MetasoundBranches module started (bank version)..."));
    // Initialization
}

void FMetasoundBranchesModule::ShutdownModule()
{

    UE_LOG(LogTemp, Log, TEXT("MetasoundBranches module shutting down..."));
    // Cleanup
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMetasoundBranchesModule, MetasoundBranches);