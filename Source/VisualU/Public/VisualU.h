// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules\ModuleManager.h"

class ISettingsModule;
class ISettingsSection;
class UVisualUSettings;

class FVisualUModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

	UVisualUSettings* GetVisualSettings() const;

private:
	ISettingsModule* SettingsModule;

	TSharedPtr<ISettingsSection> SettingsSection;
};
