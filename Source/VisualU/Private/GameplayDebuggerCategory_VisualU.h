// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#if WITH_GAMEPLAY_DEBUGGER_MENU
#include "GameplayDebuggerCategory.h"
#include "Containers/EnumAsByte.h"

class APlayerController;
class AActor;
class UTexture;

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

private:
	struct FRepData
	{
	public:
		FRepData() = default;

		FString ControllerName;
		bool bIsRendererVisualized;
		uint8 ControllerMode;
		FString CurrentSceneName;
		FString CurrentNodeName;
		FString ControllerHeadDesc;
		bool bIsControllerTransitioning;
		float ControllerAutoMoveDelay;
		bool bControllerPlaysSound;
		bool bControllerPlaysTransitions;
		int32 NumScenesToLoad;
		FString ExhaustedScenesDesc;
		FString AsyncQueueDesc;

		void Serialize(FArchive& Ar);
	};

	FRepData RepData;
};

#endif
