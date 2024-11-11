// Copyright (c) 2024 Evgeny Shustov

#pragma once

#include "CoreMinimal.h"
#if WITH_GAMEPLAY_DEBUGGER_MENU
#include "GameplayDebuggerCategory.h"

class APlayerController;
class AActor;

/**
 * Debugger for VisualU that provides information on UVisualController.
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
		FRepData();

		FString ControllerName;
		uint8 RendererVisibility;
		uint8 ControllerMode;
		FString CurrentSceneName;
		FString CurrentNodeName;
		FString CurrentSceneDesc;
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
