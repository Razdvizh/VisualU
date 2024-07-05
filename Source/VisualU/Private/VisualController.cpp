// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualController.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Engine/DataTable.h"
#include "Tasks/Task.h"
#include "GameFramework/PlayerController.h"
#include "VisualUSettings.h"
#include "VisualRenderer.h"
#include "VisualU.h"

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
static TAutoConsoleVariable<float> CVarEditorStallThreadForLoading
(
	TEXT("VisualU.EditorStallThreadForLoading"),
	0.f,
	TEXT("Editor only. Has effect when > 0.f. Sleeps process for specified amount of time during next scene loading to give GC time to catch up."),
	ECVF_Cheat
);
#endif

void UE::VisualU::Private::FFastMoveAsyncWorker::DoWork()
{
	checkf(VisualController, TEXT("Can't start fast move for invalid controller."));
	check(ControllerDirection != EVisualControllerDirection::None);
	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float DeltaTime)
	{
		if (IsValid(VisualController) && VisualController->IsFastMoving())
		{
			const bool bCanContinue = (ControllerDirection == EVisualControllerDirection::Forward
				? (!VisualController->IsCurrentScenarioHead() 
					&& !VisualController->IsWithChoice() 
					&& VisualController->RequestNextScene())
				: VisualController->RequestPreviousScene());

			if (!bCanContinue)
			{
				VisualController->ShouldPlayTransitions(bPlayedTransitions);
				VisualController->CancelFastMove();
			}

			return bCanContinue;
		}

		return false;
	}));
}

UVisualController::UVisualController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	Renderer(nullptr),
	NextSceneHandle(nullptr),
	Node(),
	SceneIndex(0),
	ScenesToLoad(5),
	SceneHandles(),
	ExhaustedScenes(),
	Head(nullptr),
	FastMoveTask(nullptr),
	bPlayTransitions(true),
	AutoMoveDelay(5.f),
	Mode(EVisualControllerMode::Idle)
{
	const UVisualUSettings* VisualUSettings = GetDefault<UVisualUSettings>();

	checkf(!VisualUSettings->FirstDataTable.IsNull(), TEXT("Unable to find first data table, please specify one in project settings."));
	const UDataTable* FirstDataTable = VisualUSettings->FirstDataTable.LoadSynchronous();

	checkf(FirstDataTable->GetRowStruct()->IsChildOf(FScenario::StaticStruct()), TEXT("Data table must be based on FScenario struct."));
	FirstDataTable->GetAllRows(UE_SOURCE_LOCATION, Node);

	checkf(Node.IsValidIndex(0), TEXT("First Data Table is empty!"));
	Head = GetCurrentScene();
}

void UVisualController::BeginDestroy()
{
	CancelFastMove();
	CancelAutoMove();
	if (Renderer)
	{
		Renderer->ForceStopTransition();
	}

	Super::BeginDestroy();
}

void UVisualController::PreSave(FObjectPreSaveContext SaveContext)
{
	CancelFastMove();
	CancelAutoMove();
	if (Renderer)
	{
		Renderer->ForceStopTransition();
	}
	
	Super::PreSave(SaveContext);
}

TSharedPtr<FStreamableHandle> UVisualController::LoadSceneAsync(const FScenario* Scene, FStreamableDelegate AfterLoadDelegate)
{
	check(Scene);

	TArray<FSoftObjectPath> DataToLoad;

	Scene->GetDataToLoad(DataToLoad);

	return UAssetManager::GetStreamableManager().RequestAsyncLoad(DataToLoad, AfterLoadDelegate, FStreamableManager::DefaultAsyncLoadPriority);
}

TSharedPtr<FStreamableHandle> UVisualController::LoadScene(const FScenario* Scene, FStreamableDelegate AfterLoadDelegate)
{
	check(Scene);

	TArray<FSoftObjectPath> DataToLoad;

	Scene->GetDataToLoad(DataToLoad);

	TSharedPtr<FStreamableHandle> Handle = UAssetManager::GetStreamableManager().RequestSyncLoad(DataToLoad, false);

	AfterLoadDelegate.ExecuteIfBound();

	return Handle;
}

void UVisualController::PrepareScenes(EVisualControllerDirection::Type Direction)
{
	check(Direction != EVisualControllerDirection::None);
	if (ScenesToLoad > 0)
	{
		if (SceneHandles.IsEmpty())
		{
			for (int32 i = 1; i <= ScenesToLoad; i++)
			{
				const int32 u = Direction == EVisualControllerDirection::Forward ? i : -i;
				if (Node.IsValidIndex(SceneIndex + u))
				{
					const FScenario* Scene = GetSceneAt(SceneIndex + u);
					TSharedPtr<FStreamableHandle> SceneHandle = LoadSceneAsync(Scene);
					
					SceneHandles.Enqueue(SceneHandle);
				}
			}

			return;
		}

		const int32 NextIndex = SceneIndex + (Direction == EVisualControllerDirection::Forward ? (ScenesToLoad + 1) : -(ScenesToLoad + 1));
		if (Node.IsValidIndex(NextIndex))
		{
			TSharedPtr<FStreamableHandle> SceneHandle = LoadSceneAsync(GetSceneAt(NextIndex));
			SceneHandles.Enqueue(MoveTemp(SceneHandle));
		}

		SceneHandles.Dequeue(NextSceneHandle);
	}
}

bool UVisualController::RequestNextScene()
{
	if (!CanAdvanceScene() || Renderer->IsTransitionInProgress())
	{
		return false;
	}
	
	OnSceneEnd.Broadcast();
	OnNativeSceneEnd.Broadcast();

	PrepareScenes();
	AssertNextSceneLoad();

	SceneIndex += 1;
	
	const FScenario* CurrentScene = GetCurrentScene();
	if (SceneIndex > Head->Index && Head->Owner == CurrentScene->Owner)
	{
		Head = CurrentScene;
	}

	if (!TryPlayTransition(GetSceneAt(SceneIndex - 1), CurrentScene))
	{
		Renderer->DrawScene(CurrentScene);
	}

	CancelNextScene();
	
	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();

	return true;
}

bool UVisualController::RequestPreviousScene()
{
	if (Renderer->IsTransitionInProgress())
	{
		return false;
	}

	if (!CanRetractScene())
	{
		if (!ExhaustedScenes.IsEmpty())
		{
			SetCurrentScene(ExhaustedScenes.Pop());
			return true;
		}

		return false;
	}

	OnSceneEnd.Broadcast();
	OnNativeSceneEnd.Broadcast();

	PrepareScenes(EVisualControllerDirection::Backward);
	AssertNextSceneLoad(EVisualControllerDirection::Backward);

	SceneIndex -= 1;
	Renderer->DrawScene(GetCurrentScene());
	
	CancelNextScene();

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();

	return true;
}

bool UVisualController::RequestScene(const FScenario* Scene)
{
	if (Renderer->IsTransitionInProgress())
	{
		return false;
	}
	check(Scene);

	bool bIsFound = false;
	if (Node[0]->Owner == Scene->Owner)
	{
		bIsFound = true;
	}
	else
	{
		/*Traverse the stack until the requested Node is found*/
		for (int32 i = ExhaustedScenes.Num() - 1; i >= 0; i--)
		{
			if (ExhaustedScenes[i]->Owner == Scene->Owner)
			{
				ExhaustedScenes.Pop();
				bIsFound = true;
				break;
			}
			ExhaustedScenes.Pop();
		}
	}

	if (bIsFound)
	{
		SetCurrentScene(Scene);
	}

	return bIsFound;
}

const FScenario* UVisualController::GetSceneAt(int32 Index)
{
	check(Node.IsValidIndex(Index));
	return Node[Index];
}

bool UVisualController::RequestScenario(const FScenario& Scenario)
{
	return RequestScene(&Scenario);
}

void UVisualController::SetCurrentScene(const FScenario* Scene)
{
	check(Scene);
	if (Scene->Owner != GetSceneAt(0)->Owner)
	{
		Node.Empty();
		Scene->Owner->GetAllRows(UE_SOURCE_LOCATION, Node);
	}

	OnSceneEnd.Broadcast();
	OnNativeSceneEnd.Broadcast();
	
	SceneHandles.Empty();
	CancelNextScene();

	SceneIndex = Scene->Index;
	LoadScene(GetCurrentScene());
	Renderer->DrawScene(GetCurrentScene());
	PrepareScenes();

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();
}

void UVisualController::AssertNextSceneLoad(EVisualControllerDirection::Type Direction)
{
	check(Direction != EVisualControllerDirection::None);
	const int32 NextSceneIndex = SceneIndex + StaticCast<int32>(Direction);
	NextSceneHandle = LoadScene(GetSceneAt(NextSceneIndex));

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST) && WITH_EDITOR
	FPlatformProcess::Sleep(CVarEditorStallThreadForLoading.GetValueOnAnyThread());
#endif
}

bool UVisualController::RequestNode(const UDataTable* NewNode)
{
	if (Renderer->IsTransitionInProgress())
	{
		return false;
	}
	check(NewNode);
	checkf(!NewNode->GetRowMap().IsEmpty(), TEXT("Jumping to empty node is not allowed."));
	checkf(GetCurrentScene()->Owner != NewNode, TEXT("Jumping to active node is not allowed."));
	checkf(NewNode->GetRowStruct()->IsChildOf(FScenario::StaticStruct()), TEXT("Node must be based on FScenario struct."));
	//Assertions aren't present in shipping builds, so compiler most likely will optimize empty loop.
	for (const FScenario* ExhaustedScene : ExhaustedScenes)
	{
		checkf(ExhaustedScene->Owner != NewNode, TEXT("Jumping to already \"seen\" nodes is invalid. Use RequestScene or RequestPreviousScene instead."));
	}
	
	FScenario* Last = Node.Last();
	ExhaustedScenes.Push(Last);

	Node.Empty();
	NewNode->GetAllRows(UE_SOURCE_LOCATION, Node);

	checkf(!Node.IsEmpty(), TEXT("Trying to jump to empty Data Table! - %s"), *NewNode->GetFName().ToString());

	OnSceneEnd.Broadcast();
	OnNativeSceneEnd.Broadcast();

	SceneHandles.Empty();
	CancelNextScene();

	SceneIndex = 0;

	Head = GetCurrentScene();
	LoadScene(Head);
	if (!TryPlayTransition(Last, Head))
	{
		Renderer->DrawScene(Head);
	}
	PrepareScenes();

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();

	return true;
}

void UVisualController::RequestFastMove(EVisualControllerDirection::Type Direction)
{
	if (!(IsAutoMoving() || IsFastMoving()))
	{
		FastMoveTask = MakeUnique<UE::VisualU::Private::FFastMoveAsyncTask>(this, Direction, bPlayTransitions);
		bPlayTransitions = false;
		Mode = EVisualControllerMode::FastMoving;
		FastMoveTask->StartBackgroundTask();
	}
}

void UVisualController::RequestAutoMove(EVisualControllerDirection::Type Direction)
{
	if (!(IsAutoMoving() || IsFastMoving()))
	{
		Mode = EVisualControllerMode::AutoMoving;
		const auto AutoMove = [this, Direction](float DeltaTime)
		{
			if (IsAutoMoving())
			{
				const bool bCanContinue = (Direction == EVisualControllerDirection::Forward 
					? (!IsWithChoice() && RequestNextScene())
					: RequestPreviousScene());

				if (!bCanContinue)
				{
					CancelAutoMove();
				}

				return bCanContinue;
			}

			return false;
		};

		/*Fire it once first to replicate a do-while style*/
		FTSTicker::FDelegateHandle Handle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda(AutoMove), 0.f);
		FTSTicker::RemoveTicker(Handle);
		FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda(AutoMove), AutoMoveDelay);
	}
}

void UVisualController::CancelFastMove()
{
	if (IsFastMoving())
	{
		if (FastMoveTask.IsValid())
		{
			FastMoveTask->EnsureCompletion(/*bDoWorkOnThisThreadIfNotStarted=*/false);
			FastMoveTask->Cancel();
			FastMoveTask.Reset(nullptr);
		}

		Mode = EVisualControllerMode::Idle;
	}
}

void UVisualController::CancelAutoMove()
{
	if (IsAutoMoving())
	{
		Mode = EVisualControllerMode::Idle;
	}
}

bool UVisualController::TryPlayTransition(const FScenario* From, const FScenario* To)
{
	if (bPlayTransitions && !Renderer->IsTransitionInProgress())
	{
		check(Renderer);
		return Renderer->TryDrawTransition(From, To);
	}

	return false;
}

void UVisualController::Visualize(TSubclassOf<UVisualRenderer> RendererClass, int32 ZOrder)
{
	if (!IsValid(Renderer))
	{
		Renderer = CreateWidget<UVisualRenderer>(GetOuterAPlayerController(), RendererClass);
	}

	LoadScene(GetCurrentScene());
	Renderer->AddToPlayerScreen(ZOrder);
	Renderer->DrawScene(GetCurrentScene());
	PrepareScenes();
}

void UVisualController::Discard()
{
	check(Renderer);
	Renderer->RemoveFromParent();
}

void UVisualController::SetVisibility(ESlateVisibility Visibility)
{
	check(Renderer);
	Renderer->SetVisibility(Visibility);
}

void UVisualController::SetNumScenesToLoad(int32 Num)
{
	if (ensureMsgf(Num >= 0, TEXT("Expected positive value, set operation failed.")))
	{
		if (Num > 100)
		{
			UE_LOG(LogVisualU, Warning, TEXT("Received large (%i) request for scene loading, it might have a significant impact on performance."), Num);
		}

		ScenesToLoad = Num;
	}
}

void UVisualController::ShouldPlayTransitions(bool bShouldPlay)
{
	bPlayTransitions = bShouldPlay;
}

void UVisualController::SetAutoMoveDelay(float Delay)
{
	if (ensureMsgf(!FMath::IsNegativeOrNegativeZero(Delay), TEXT("Negative time is invalid, set operation failed.")))
	{
		AutoMoveDelay = Delay;
	}
}

bool UVisualController::IsCurrentScenarioHead() const
{
	return GetCurrentScene() == Head;
}

bool UVisualController::IsWithChoice() const
{
	return GetCurrentScene()->HasChoice();
}

void UVisualController::CancelNextScene()
{
	if (NextSceneHandle.IsValid())
	{
		NextSceneHandle->CancelHandle();
		NextSceneHandle.Reset();
	}
}

bool UVisualController::CanAdvanceScene() const
{
	return Node.IsValidIndex(SceneIndex + 1);
}

bool UVisualController::CanRetractScene() const
{
	return Node.IsValidIndex(SceneIndex - 1);
}

bool UVisualController::IsSceneExhausted(const FScenario* Scene) const
{
	return ExhaustedScenes.Contains(Scene);
}

bool UVisualController::IsScenarioExhausted(const FScenario& Scenario) const
{
	return IsSceneExhausted(&Scenario);
}

const FScenario* UVisualController::GetCurrentScene() const
{
	return Node[SceneIndex];
}

const FScenario& UVisualController::GetCurrentScenario() const
{
	return *GetCurrentScene();
}
