// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#if WITH_GAMEPLAY_DEBUGGER_MENU
#include "GameplayDebuggerCategory.h"

class APlayerController;
class AActor;

/**
 * Debugger for VisualU that provides information on Visual controller.
 */
class FGameplayDebuggerCategory_VisualU final : public FGameplayDebuggerCategory
{
public:
	FGameplayDebuggerCategory_VisualU();

	virtual void CollectData(APlayerController* OwnerPC, AActor* DebugActor) override;
	virtual void DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext) override;

	VISUALU_API static TSharedRef<FGameplayDebuggerCategory> MakeInstance();
};

#endif
