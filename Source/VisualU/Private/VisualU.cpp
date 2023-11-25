// Copyright Epic Games, Inc. All Rights Reserved.

#include "VisualU.h"
#include "VisualUSettings.h"
#if WITH_EDITOR
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Developer/Settings/Public/ISettingsSection.h"
#endif

#define LOCTEXT_NAMESPACE "VisualUModule"
DEFINE_LOG_CATEGORY(LogVisualU);

void FVisualUModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	#if WITH_EDITOR
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings"));
	SettingsSection = SettingsModule->RegisterSettings(
		TEXT("Project"),
		TEXT("Plugins"),
		TEXT("VisualUSettings"),
		LOCTEXT("VisualUSettingsName", "Visual U"),
		LOCTEXT("VisualUSettingsDescription", "Configure options for Visual U elements"),
		GetMutableDefault<UVisualUSettings>());
	#endif
}

void FVisualUModule::ShutdownModule()
{
	#if WITH_EDITOR
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings")))
	{
		SettingsModule->UnregisterSettings(TEXT("Project"), TEXT("Plugins"), TEXT("VisualUSettings"));
		SettingsSection.Reset();
	}
	#endif
}

#if WITH_EDITOR
const UVisualUSettings* FVisualUModule::GetVisualSettings() const
{
	return StaticCast<UVisualUSettings*>(SettingsSection->GetSettingsObject().Get());
}
#endif

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVisualUModule, VisualU)
