// Copyright Epic Games, Inc. All Rights Reserved.

#include "VisualU.h"
#include "VisualUSettings.h"
#include "Developer\Settings\Public\ISettingsModule.h"
#include "Developer\Settings\Public\ISettingsSection.h"

#define LOCTEXT_NAMESPACE "FVisualUModule"

void FVisualUModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings"));
	SettingsSection = SettingsModule->RegisterSettings(
		TEXT("Project"),
		TEXT("Plugins"),
		TEXT("VisualUSettings"),
		LOCTEXT("VisualUSettingsName", "Visual U"),
		LOCTEXT("VisualUSettingsDescription", "Configure options for Visual U elements"),
		GetMutableDefault<UVisualUSettings>());
}

void FVisualUModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

UVisualUSettings* FVisualUModule::GetVisualSettings() const
{
	return Cast<UVisualUSettings>(SettingsSection->GetSettingsObject().Get());
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVisualUModule, VisualU)