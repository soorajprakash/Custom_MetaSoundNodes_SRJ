#include "MetasoundBranches/Public/MetasoundBranches.h"
#include "MetasoundFrontendRegistries.h"
#include "Modules/ModuleManager.h"
//#include "MetasoundBranches/Public/MetasoundSahNode.h"

#define LOCTEXT_NAMESPACE "FMetasoundBranchesModule"

void FMetasoundBranchesModule::StartupModule()
{

    UE_LOG(LogTemp, Log, TEXT("MetasoundBranches module started..."));
	// Appears to be a duplicate of the registration in MetasoundSahNode.cpp
    //FMetasoundFrontendRegistryContainer::Get()->RegisterNode(MetasoundSahNode);
    
    // Initialization
}

void FMetasoundBranchesModule::ShutdownModule()
{

    UE_LOG(LogTemp, Log, TEXT("MetasoundBranches module shutting down..."));
    // Appears to be a duplicate of the unregistration in MetasoundSahNode.cpp.....
    //FMetasoundFrontendRegistryContainer::Get()->UnregisterNode(MetasoundSahNode);

    // Cleanup
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMetasoundBranchesModule, MetasoundBranches);