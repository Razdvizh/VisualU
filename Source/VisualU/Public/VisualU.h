// Copyright (c) 2024 Evgeny Shustov

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class ISettingsSection;
class UVisualUSettings;

/**
* VisualU custom log category.
*/
DECLARE_LOG_CATEGORY_EXTERN(LogVisualU, Display, All);

/**
* VisualU plugin runtime module.
*/
class FVisualUModule : public IModuleInterface
{
public:
	/**
	* Registers UVisualUSettings and gameplay debugger category.
	*/
	virtual void StartupModule() override;

	/**
	* Unregisters UVisualUSettings and gameplay debugger category.
	*/
	virtual void ShutdownModule() override;

#if WITH_EDITOR
	/**
	* Editor only.
	* 
	* Getter for UVisualUSettings.
	* 
	* @note might be invalid if called before the module is loaded
	* 
	* @return VisualU plugin settings
	*/
	const UVisualUSettings* GetVisualSettings() const;

private:
	/**
	* VisualU settings section in the project settings.
	* 
	* @note located under 'Plugins' category
	*/
	TSharedPtr<ISettingsSection> SettingsSection;
#endif
};
