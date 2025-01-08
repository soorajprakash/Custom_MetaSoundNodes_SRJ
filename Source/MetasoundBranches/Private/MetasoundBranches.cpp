// Copyright Charles Matthews. All Rights Reserved.

#include "MetasoundBranches/Public/MetasoundBranches.h"
#include "MetasoundFrontendRegistries.h"
#include "Modules/ModuleManager.h"
#include "MetasoundDataTypeRegistrationMacro.h"

#define LOCTEXT_NAMESPACE "FMetasoundBranchesModule"

void FMetasoundBranchesModule::StartupModule()
{
    // Initialization
    // UE_LOG(LogTemp, Log, TEXT("MetasoundBranches module started..."));
    using namespace Metasound;

	FMetasoundFrontendRegistryContainer::Get()->RegisterPendingNodes();
}

void FMetasoundBranchesModule::ShutdownModule()
{
    // Cleanup
    // UE_LOG(LogTemp, Log, TEXT("MetasoundBranches module shutting down..."));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMetasoundBranchesModule, MetasoundBranches);