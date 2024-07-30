// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayDebuggerCategory_VisualU.h"
#if WITH_GAMEPLAY_DEBUGGER_MENU

FGameplayDebuggerCategory_VisualU::FGameplayDebuggerCategory_VisualU()
{
	bShowOnlyWithDebugActor = false;
}

void FGameplayDebuggerCategory_VisualU::CollectData(APlayerController* OwnerPC, AActor* DebugActor)
{	

}

void FGameplayDebuggerCategory_VisualU::DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext)
{
}

TSharedRef<FGameplayDebuggerCategory> FGameplayDebuggerCategory_VisualU::MakeInstance()
{
	return MakeShareable(new FGameplayDebuggerCategory_VisualU());
}

#endif

