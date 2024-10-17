#pragma once

#include "CoreMinimal.h"

class FMetasoundBranchesModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
