// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayDebuggerCategory_VisualU.h"
#if WITH_GAMEPLAY_DEBUGGER_MENU
#include "GameplayDebuggerTypes.h"
#include "CanvasItem.h"
#include "Engine/Canvas.h"
#include "Engine/Texture.h"
#include "Engine/DataTable.h"
#include "Engine/StreamableManager.h"
#include "UObject/ConstructorHelpers.h"
#include "VisualController.h"
#include "VisualRenderer.h"
#include "VisualControllerInterface.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "Containers/Queue.h"

FGameplayDebuggerCategory_VisualU::FGameplayDebuggerCategory_VisualU()
{
	bShowOnlyWithDebugActor = false;
	SetDataPackReplication<FGameplayDebuggerCategory_VisualU::FRepData>(&RepData);
}

void FGameplayDebuggerCategory_VisualU::FRepData::Serialize(FArchive& Ar)
{
	Ar << ControllerName;
	Ar.SerializeBits(&bIsRendererVisualized, 1);
	Ar.SerializeBits(&ControllerMode, 1);
	Ar << CurrentSceneName;
	Ar << CurrentNodeName;
	Ar << ControllerHeadDesc;
	Ar.SerializeBits(&bIsControllerTransitioning, 1);
	Ar << ControllerAutoMoveDelay;
	Ar.SerializeBits(&bControllerPlaysSound, 1);
	Ar.SerializeBits(&bControllerPlaysTransitions, 1);
	Ar << NumScenesToLoad;
	Ar << ExhaustedScenesDesc;
	Ar << AsyncQueueDesc;
}

void FGameplayDebuggerCategory_VisualU::CollectData(APlayerController* OwnerPC, AActor* DebugActor)
{
	if (DebugActor)
	{
		if (AController* Controller = Cast<AController>(DebugActor))
		{
			UVisualController* VisualController = Controller->Implements<UVisualControllerInterface>()
				? IVisualControllerInterface::Execute_GetVisualController(Controller)
				: Cast<UVisualController>((UObject*)FindObjectWithOuter(Controller, UVisualController::StaticClass()));

			if (VisualController)
			{
				RepData.ControllerName = VisualController->GetFName().ToString();
				RepData.bIsRendererVisualized = VisualController->IsVisualized();
				RepData.ControllerMode = StaticCast<uint8>(VisualController->GetMode());

				const FScenario* CurrentScene = VisualController->GetCurrentScene();
				if (ensure(CurrentScene && CurrentScene->GetOwner()))
				{
					TArray<FName> SceneNames = CurrentScene->GetOwner()->GetRowNames();
					RepData.CurrentSceneName = SceneNames.IsValidIndex(CurrentScene->GetIndex())
						? SceneNames[CurrentScene->GetIndex()].ToString()
						: TEXT("None");

					RepData.CurrentNodeName = CurrentScene->GetOwner()->GetFName().ToString();
				}
				else
				{
					RepData.CurrentSceneName = TEXT("Invalid");
					RepData.CurrentNodeName = TEXT("Invalid");
				}

				RepData.ControllerHeadDesc = VisualController->GetHeadDebugString();
				RepData.bIsControllerTransitioning = VisualController->IsTransitioning();
				RepData.ControllerAutoMoveDelay = VisualController->GetAutoMoveDelay();
				RepData.bControllerPlaysSound = VisualController->PlaysSound();
				RepData.bControllerPlaysTransitions = VisualController->PlaysTransitions();
				RepData.NumScenesToLoad = VisualController->GetNumScenesToLoad();
				RepData.ExhaustedScenesDesc = VisualController->GetExhaustedScenesDebugString();
				RepData.AsyncQueueDesc = VisualController->GetAsyncQueueDebugString();
			}
		}
	}
}

void FGameplayDebuggerCategory_VisualU::DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext)
{
	if (!RepData.ControllerName.IsEmpty())
	{
		UCanvas* Canvas = CanvasContext.Canvas.Get();
		
		CanvasContext.Printf(TEXT("{blue}Visual Controller: {magenta}%s"), *RepData.ControllerName);
		CanvasContext.Printf(TEXT("{blue}Is Renderer visualized: {magenta}%s"), RepData.bIsRendererVisualized ? TEXT("true") : TEXT("false"));
		CanvasContext.Printf(TEXT("{blue}Controller mode: {magenta}%s"), *UEnum::GetValueAsString(StaticCast<EVisualControllerMode>(RepData.ControllerMode)));
		CanvasContext.Printf(TEXT("{blue}Current scenario: {magenta}%s"), *RepData.CurrentSceneName);
		CanvasContext.Printf(TEXT("{blue}Current node: {magenta}%s"), *RepData.CurrentNodeName);
		CanvasContext.Printf(TEXT("{blue}Controller head: {magenta}%s"), *RepData.ControllerHeadDesc);
		CanvasContext.Printf(TEXT("{blue}Is Controller transitioning: {magenta}%s"), RepData.bIsControllerTransitioning ? TEXT("true") : TEXT("false"));
		CanvasContext.Printf(TEXT("{blue}Controller auto move delay: {magenta}%.2f"), RepData.ControllerAutoMoveDelay);
		CanvasContext.Printf(TEXT("{blue}Controller plays sound: {magenta}%s"), RepData.bControllerPlaysSound? TEXT("true") : TEXT("false"));
		CanvasContext.Printf(TEXT("{blue}Controller plays transitions: {magenta}%s"), RepData.bControllerPlaysTransitions ? TEXT("true") : TEXT("false"));
		CanvasContext.Printf(TEXT("{blue}Controller number of scenarios to load: {magenta}%i"), RepData.NumScenesToLoad);
		CanvasContext.Printf(TEXT("{blue}[Exhausted scenarios]\n{magenta}%s"), *RepData.ExhaustedScenesDesc);
		CanvasContext.Printf(TEXT("{blue}[Asynchronous queue]\n{magenta}%s"), *RepData.AsyncQueueDesc);
	}
}

TSharedRef<FGameplayDebuggerCategory> FGameplayDebuggerCategory_VisualU::MakeInstance()
{
	return MakeShareable(new FGameplayDebuggerCategory_VisualU());
}

#endif
