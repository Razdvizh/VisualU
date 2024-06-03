#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FVisualUDebuggerModule : public IModuleInterface
{
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

};