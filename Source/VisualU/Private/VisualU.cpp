// Copyright Epic Games, Inc. All Rights Reserved.

#include "VisualU.h"
#include "VisualUSettings.h"
#include "Developer\Settings\Public\ISettingsModule.h"
#include "Developer\Settings\Public\ISettingsSection.h"

#define LOCTEXT_NAMESPACE "FVisualUModule"
DEFINE_LOG_CATEGORY(LogVisualU);

void FVisualUModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings"));
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
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings"));
	if (SettingsModule)
	{
		SettingsModule->UnregisterSettings(TEXT("Project"), TEXT("Plugins"), TEXT("VisualUSettings"));
		SettingsSection.Reset();
	}
}

const UVisualUSettings* FVisualUModule::GetVisualSettings() const
{
	return Cast<UVisualUSettings>(SettingsSection->GetSettingsObject().Get());
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVisualUModule, VisualU)