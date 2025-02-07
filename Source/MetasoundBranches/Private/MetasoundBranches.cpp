// Copyright 2025 Charles Matthews. All Rights Reserved.

#include "MetasoundBranches/Public/MetasoundBranches.h"
#include "MetasoundFrontendRegistries.h"
#include "Modules/ModuleManager.h"
#include "MetasoundDataTypeRegistrationMacro.h"

#define LOCTEXT_NAMESPACE "FMetasoundBranchesModule"

namespace Metasound
{
    namespace StandardNodes
    {
        const FName Namespace = "UE";
        const FName AudioVariant = "Audio";
    }
}

void FMetasoundBranchesModule::StartupModule()
{
    using namespace Metasound;
	FMetasoundFrontendRegistryContainer::Get()->RegisterPendingNodes();
}

void FMetasoundBranchesModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMetasoundBranchesModule, MetasoundBranches);