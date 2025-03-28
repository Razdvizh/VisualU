// Copyright (c) 2024 Evgeny Shustov

#include "VisualU.h"
#include "VisualUSettings.h"
#if WITH_EDITOR
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Developer/Settings/Public/ISettingsSection.h"
#endif
#if WITH_GAMEPLAY_DEBUGGER_CORE
#include "GameplayDebugger.h"
#endif
#if WITH_GAMEPLAY_DEBUGGER
#include "GameplayDebuggerCategory_VisualU.h"
#endif

#define LOCTEXT_NAMESPACE "VisualU"
DEFINE_LOG_CATEGORY(LogVisualU);

void FVisualUModule::StartupModule()
{
#if WITH_EDITOR
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings")))
	{
		SettingsSection = SettingsModule->RegisterSettings(
		TEXT("Project"),
		TEXT("Plugins"),
		TEXT("VisualUSettings"),
		LOCTEXT("VisualUSettingsName", "VisualU"),
		LOCTEXT("VisualUSettingsDescription", "Configure options for VisualU elements"),
		GetMutableDefault<UVisualUSettings>());
	}
#endif
#if WITH_GAMEPLAY_DEBUGGER
	IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
	GameplayDebuggerModule.RegisterCategory(TEXT("VisualU"), IGameplayDebugger::FOnGetCategory::CreateStatic(&FGameplayDebuggerCategory_VisualU::MakeInstance));
	GameplayDebuggerModule.NotifyCategoriesChanged();
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
#if WITH_GAMEPLAY_DEBUGGER
	if (IGameplayDebugger::IsAvailable())
	{
		IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
		GameplayDebuggerModule.UnregisterCategory(TEXT("VisualU"));
		GameplayDebuggerModule.NotifyCategoriesChanged();
	}
#endif
}

#if WITH_EDITOR
const UVisualUSettings* FVisualUModule::GetVisualSettings() const
{
	if (SettingsSection.IsValid())
	{
		return StaticCast<UVisualUSettings*>(SettingsSection->GetSettingsObject().Get());
	}
	
	return nullptr;
}
#endif

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVisualUModule, VisualU)
