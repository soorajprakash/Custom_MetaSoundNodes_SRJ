#include "MetasoundBranches/Public/MetasoundBranches.h"
#include "MetasoundFrontendRegistries.h"
#include "Modules/ModuleManager.h"
//#include "MetasoundSahNode.h"

#define LOCTEXT_NAMESPACE "FMetasoundBranchesModule"

void FMetasoundBranchesModule::StartupModule()
{

    UE_LOG(LogTemp, Log, TEXT("MetasoundBranches module started..."));
    FMetasoundFrontendRegistryContainer::Get()->RegisterNode<MetasoundSahNode>();
    
    // Initialization
}

void FMetasoundBranchesModule::ShutdownModule()
{

    UE_LOG(LogTemp, Log, TEXT("MetasoundBranches module shutting down..."));
    FMetasoundFrontendRegistryContainer::Get()->UnregisterNode<MetasoundSahNode>();

    // Cleanup
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMetasoundBranchesModule, MetasoundBranches);