#include "VisualUDebugger.h"

#if WITH_GAMEPLAY_DEBUGGER_CORE
#include "GameplayDebugger.h"
#endif
#if WITH_GAMEPLAY_DEBUGGER
#include "GameplayDebuggerCategory_VisualU.h"
#endif

IMPLEMENT_MODULE(FVisualUDebuggerModule, VisualUDebugger);

void FVisualUDebuggerModule::StartupModule()
{
	#if WITH_GAMEPLAY_DEBUGGER
	IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
	GameplayDebuggerModule.RegisterCategory(TEXT("VisualU"), IGameplayDebugger::FOnGetCategory::CreateStatic(&FGameplayDebuggerCategory_VisualU::MakeInstance));
	GameplayDebuggerModule.NotifyCategoriesChanged();
	#endif
}

void FVisualUDebuggerModule::ShutdownModule()
{
	#if WITH_GAMEPLAY_DEBUGGER
	if (IGameplayDebugger::IsAvailable())
	{
		IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
		GameplayDebuggerModule.UnregisterCategory(TEXT("VisualU"));
		GameplayDebuggerModule.NotifyCategoriesChanged();
	}
	#endif
}
