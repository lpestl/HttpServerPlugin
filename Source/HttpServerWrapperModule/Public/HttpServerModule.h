#pragma once

#include "Modules/ModuleManager.h"
#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHttpServerPlugin, Log, All);

class FHttpServerWrapperModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
