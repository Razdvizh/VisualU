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

FGameplayDebuggerCategory_VisualU::FRepData::FRepData() = default;

void FGameplayDebuggerCategory_VisualU::FRepData::Serialize(FArchive& Ar)
{
	Ar << ControllerName;
	Ar.SerializeBits(&bIsRendererVisualized, 1);
	Ar.SerializeBits(&ControllerMode, 1);
	Ar << CurrentSceneName;
	Ar << CurrentNodeName;
	Ar << CurrentSceneDesc;
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
					RepData.CurrentSceneDesc = CurrentScene->ToString();
				}
				else
				{
					RepData.CurrentSceneName = TEXT("Invalid");
					RepData.CurrentNodeName = TEXT("Invalid");
					RepData.CurrentSceneDesc = TEXT("Invalid");
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
		check(Canvas);

		const float LineHeight = CanvasContext.GetLineHeight();
		const float CanvasX = Canvas->SizeX / Canvas->GetDPIScale();
		const float InitialCategoryHeaderPosY = CanvasContext.CursorY - LineHeight;
		CanvasContext.Printf(TEXT("{cyan}Visual Controller: {magenta}%s"), *RepData.ControllerName);
		CanvasContext.Printf(TEXT("{cyan}Is Renderer visualized: {magenta}%s"), RepData.bIsRendererVisualized ? TEXT("true") : TEXT("false"));
		CanvasContext.Printf(TEXT("{cyan}Controller mode: {magenta}%s"), *StaticEnum<EVisualControllerMode>()->GetAuthoredNameStringByValue(RepData.ControllerMode));
		CanvasContext.Printf(TEXT("{cyan}Current scenario: {magenta}%s"), *RepData.CurrentSceneName);
		CanvasContext.Printf(TEXT("{cyan}Current node: {magenta}%s"), *RepData.CurrentNodeName);
		CanvasContext.Printf(TEXT("{cyan}Controller head: {magenta}%s"), *RepData.ControllerHeadDesc);
		CanvasContext.Printf(TEXT("{cyan}Is Controller transitioning: {magenta}%s"), RepData.bIsControllerTransitioning ? TEXT("true") : TEXT("false"));
		CanvasContext.Printf(TEXT("{cyan}Controller auto move delay: {magenta}%.2f"), RepData.ControllerAutoMoveDelay);
		CanvasContext.Printf(TEXT("{cyan}Controller plays sound: {magenta}%s"), RepData.bControllerPlaysSound? TEXT("true") : TEXT("false"));
		CanvasContext.Printf(TEXT("{cyan}Controller plays transitions: {magenta}%s"), RepData.bControllerPlaysTransitions ? TEXT("true") : TEXT("false"));
		CanvasContext.Printf(TEXT("{cyan}Controller number of scenarios to load: {magenta}%i"), RepData.NumScenesToLoad);
		CanvasContext.Printf(TEXT("{cyan}[Exhausted scenarios]\n{magenta}%s"), *RepData.ExhaustedScenesDesc);
		CanvasContext.Printf(TEXT("{cyan}[Asynchronous queue]\n{magenta}%s"), *RepData.AsyncQueueDesc);

		float CurrentSceneDescX = 0.f, CurrentSceneDescY = 0.f;
		const float Padding = 5.f;
		CanvasContext.MeasureString(RepData.CurrentSceneDesc, CurrentSceneDescX, CurrentSceneDescY);
		CanvasContext.PrintfAt(CanvasX - Padding - CurrentSceneDescX, InitialCategoryHeaderPosY, TEXT("{cyan}[Current scenario info]"));
		CanvasContext.PrintfAt(CanvasX - Padding - CurrentSceneDescX, InitialCategoryHeaderPosY + LineHeight, TEXT("{magenta}%s"), *RepData.CurrentSceneDesc);
	}
}

TSharedRef<FGameplayDebuggerCategory> FGameplayDebuggerCategory_VisualU::MakeInstance()
{
	return MakeShareable(new FGameplayDebuggerCategory_VisualU());
}

#endif
