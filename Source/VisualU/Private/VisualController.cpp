// Copyright (c) 2024 Evgeny Shustov


#include "VisualController.h"
#include "Engine/Engine.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Engine/DataTable.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Misc/App.h"
#include "Tasks/Task.h"
#include "Containers/Queue.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "Components/WidgetComponent.h"
#include "Sound/SoundBase.h"
#include "VisualVersioningSubsystem.h"
#include "VisualUCustomVersion.h"
#include "VisualUSettings.h"
#include "VisualRenderer.h"
#include "VisualU.h"

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
				VisualController->ShouldPlaySound(bPlayedSound);
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
	SceneHandles(),
	NodeReferenceKeeper(),
	ExhaustedScenes(),
	Head(nullptr),
	FastMoveTask(nullptr),
	AutoMoveHandle(),
	ScenesToLoad(5),
	bPlayTransitions(true),
	bPlaySound(true),
	AutoMoveDelay(5.f),
	Mode(EVisualControllerMode::Idle)
{
}

void UVisualController::BeginDestroy()
{
	CancelFastMove();
	CancelAutoMove();
	if (Renderer)
	{
		Renderer->ForceStopTransition();
		Renderer->RemoveFromParent();
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

void UVisualController::PostInitProperties()
{
	Super::PostInitProperties();

	bPlaySound &= (GEngine && GEngine->UseSound());

	if (!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
	{
		auto Initialization = [this](APlayerController* PlayerController)
		{
			APlayerController* OwningPlayerController = GetOuterAPlayerController();
			if (OwningPlayerController == PlayerController)
			{
				const UVisualUSettings* VisualUSettings = GetDefault<UVisualUSettings>();
				check(VisualUSettings);

				checkf(!VisualUSettings->FirstDataTable.IsNull(), TEXT("Unable to find first data table, please specify one in project settings."));
				const UDataTable* FirstDataTable = VisualUSettings->FirstDataTable.LoadSynchronous();

				checkf(FirstDataTable->GetRowStruct()->IsChildOf(FScenario::StaticStruct()), TEXT("Data table must be based on FScenario struct."));
				FirstDataTable->GetAllRows(UE_SOURCE_LOCATION, Node);

				checkf(Node.IsValidIndex(0), TEXT("First Data Table is empty!"));
				Head = GetCurrentScene();
				NodeReferenceKeeper.Add(FirstDataTable);

				OnSceneStart.Broadcast(*Head);

				Renderer = CreateWidget<UVisualRenderer>(OwningPlayerController, UVisualRenderer::StaticClass());
				check(Renderer);

				/*Rebuild widget immediately to create renderer widgets*/
				Renderer->TakeWidget();

				TSharedPtr<FStreamableHandle> HeadHandle = LoadScene(Head);
				Renderer->DrawScene(Head);
				TryPlaySceneSound(Head->Info.Sound);
				PrepareScenes();
			}
		};

		if (UWorld* World = GetWorld(); World && World->HasBegunPlay())
		{
			Initialization(GetOuterAPlayerController());
		}
		else
		{
			FGameModeEvents::GameModePostLoginEvent.AddWeakLambda(this, [this, Initialization](AGameModeBase*, APlayerController* PlayerController)
			{
				Initialization(PlayerController);

				FGameModeEvents::GameModePostLoginEvent.RemoveAll(this);
			});
		}
	}
}

void UVisualController::SerializeController(FArchive& Ar)
{
	check(Renderer);

	Ar.UsingCustomVersion(FVisualUCustomVersion::GUID);
	Ar.ArIsSaveGame = true;

	Super::Serialize(Ar);

	if (Ar.IsSaving())
	{
		int32 NumExhaustedScenes = ExhaustedScenes.Num();
		Ar << NumExhaustedScenes;
		for (FScenario*& ExhaustedScene : ExhaustedScenes)
		{
			Ar << *ExhaustedScene;
		}

		Ar << const_cast<FScenario&>(GetCurrentScenario());
		Ar << *(const_cast<FScenario*>(Head));
	}
	else
	{
		int32 NumExhaustedScenes = 0;
		Ar << NumExhaustedScenes;
		ExhaustedScenes.Reserve(NumExhaustedScenes);
		for (int32 i = 0; i < NumExhaustedScenes; i++)
		{
			FScenario ExhaustedScene;
			Ar << ExhaustedScene;
			ExhaustedScenes.Add(FScenario::ResolveScene(ExhaustedScene));
		}

		FScenario CurrentScenario;
		Ar << CurrentScenario;
		if (CurrentScenario.GetOwner())
		{
			Node.Empty();
			CurrentScenario.GetOwner()->GetAllRows(UE_SOURCE_LOCATION, Node);
		}
		SceneIndex = CurrentScenario.GetIndex();

		FScenario SavedHead;
		Ar << SavedHead;
		Head = FScenario::ResolveScene(SavedHead);

		const FScenario* CurrentScene = GetCurrentScene();
		TSharedPtr<FStreamableHandle> CurrentSceneHandle = LoadScene(CurrentScene);
		Renderer->DrawScene(CurrentScene);
		TryPlaySceneSound(CurrentScene->Info.Sound);
		PrepareScenes();

		OnSceneStart.Broadcast(*CurrentScene);
	}
}

bool UVisualController::RequestNextScene()
{
	check(Renderer);
	if (!CanAdvanceScene() || IsTransitioning())
	{
		return false;
	}

	OnSceneEnd.Broadcast(GetCurrentScenario());

	PrepareScenes();
	AssertNextSceneLoad();

	SceneIndex += 1;

	const FScenario* CurrentScene = GetCurrentScene();
	if (SceneIndex > Head->GetIndex() && Head->GetOwner() == CurrentScene->GetOwner())
	{
		Head = CurrentScene;
	}

	if (!TryPlayTransition(GetSceneAt(SceneIndex - 1), CurrentScene))
	{
		Renderer->DrawScene(CurrentScene);
	}

	TryPlaySceneSound(CurrentScene->Info.Sound);
	CancelNextScene();

	OnSceneStart.Broadcast(*CurrentScene);

	return true;
}

bool UVisualController::RequestPreviousScene()
{
	check(Renderer);
	if (IsTransitioning())
	{
		return false;
	}

	if (!CanRetractScene())
	{
		if (!ExhaustedScenes.IsEmpty())
		{
			if (UVisualVersioningSubsystem* VisualVersioning = TryGetVisualVersioningSubsystem())
			{
				VisualVersioning->Checkout(const_cast<FScenario*>(GetCurrentScene()));
			}
			FScenario* Scene = ExhaustedScenes.Pop();
			RollbackTo(Scene);
			return true;
		}

		return false;
	}

	if (UVisualVersioningSubsystem* VisualVersioning = TryGetVisualVersioningSubsystem())
	{
		VisualVersioning->Checkout(const_cast<FScenario*>(GetCurrentScene()));
	}

	OnSceneEnd.Broadcast(GetCurrentScenario());

	PrepareScenes(EVisualControllerDirection::Backward);
	AssertNextSceneLoad(EVisualControllerDirection::Backward);

	SceneIndex -= 1;

	const FScenario* CurrentScene = GetCurrentScene();
	Renderer->DrawScene(CurrentScene);

	CancelNextScene();

	OnSceneStart.Broadcast(*CurrentScene);

	return true;
}

bool UVisualController::RequestAdjacentScene(EVisualControllerDirection::Type Direction)
{
	check(Direction != EVisualControllerDirection::None);
	if (Direction == EVisualControllerDirection::Forward)
	{
		return RequestNextScene();
	}

	return RequestPreviousScene();
}

bool UVisualController::RequestScene(const FScenario* Scene)
{
	if (IsTransitioning())
	{
		return false;
	}

	check(Scene);
	bool bIsFound = false;
	if (ensureMsgf(!(Scene->GetOwner() == Head->GetOwner() && Scene->GetIndex() > Head->GetIndex()), TEXT("Only \"seen\" scene can be requested - %s"), *Scene->GetDebugString()))
	{
		if (Node[0]->GetOwner() == Scene->GetOwner())
		{
			bIsFound = true;
		}
		else
		{
			UVisualVersioningSubsystem* VisualVersioning = TryGetVisualVersioningSubsystem();
			/*Traverse the stack until the requested Node is found*/
			for (int32 i = ExhaustedScenes.Num() - 1; i >= 0; i--)
			{
				if (ExhaustedScenes[i]->GetOwner() == Scene->GetOwner())
				{
					FScenario* ExhaustedScene = ExhaustedScenes.Pop();
					if (VisualVersioning)
					{
						VisualVersioning->CheckoutAll(ExhaustedScene->GetOwner());
					}
					bIsFound = true;
					break;
				}
				FScenario* ExhaustedScene = ExhaustedScenes.Pop();
				NodeReferenceKeeper.Remove(ExhaustedScene->GetOwner());
				if (VisualVersioning)
				{
					VisualVersioning->CheckoutAll(ExhaustedScene->GetOwner());
				}
			}
		}

		if (bIsFound)
		{
			RollbackTo(Scene);
		}
	}

	return bIsFound;
}

bool UVisualController::RequestScenario(const FScenario& Scenario)
{
	return RequestScene(&Scenario);
}

const FScenario* UVisualController::GetSceneAt(int32 Index)
{
	check(Node.IsValidIndex(Index));
	return Node[Index];
}

bool UVisualController::RequestNode(const UDataTable* NewNode)
{
	if (IsTransitioning() || !IsIdle())
	{
		return false;
	}
	check(Renderer);
	check(NewNode);
	checkf(!NewNode->GetRowMap().IsEmpty(), TEXT("Requesting empty node is not allowed."));
	checkf(GetCurrentScene()->GetOwner() != NewNode, TEXT("Requesting active node is not allowed."));
	checkf(NewNode->GetRowStruct()->IsChildOf(FScenario::StaticStruct()), TEXT("Node must be based on FScenario struct."));
#if !UE_BUILD_SHIPPING
	for (const FScenario* ExhaustedScene : ExhaustedScenes)
	{
		checkf(ExhaustedScene->GetOwner() != NewNode, TEXT("Requesting already \"seen\" nodes is invalid. Use RequestScene or RequestPreviousScene instead."));
	}
#endif

	FScenario* Last = Node[SceneIndex];
	ExhaustedScenes.Push(Last);

	Node.Empty();
	NewNode->GetAllRows(UE_SOURCE_LOCATION, Node);

	checkf(!Node.IsEmpty(), TEXT("Trying to jump to empty Data Table! - %s"), *NewNode->GetFName().ToString());

	OnSceneEnd.Broadcast(*Last);

	SceneHandles.Empty();
	CancelNextScene();

	SceneIndex = 0;

	Head = GetCurrentScene();
	NodeReferenceKeeper.Add(Head->GetOwner());
	TSharedPtr<FStreamableHandle> CurrentSceneHandle = LoadScene(Head);
	if (!TryPlayTransition(Last, Head))
	{
		Renderer->DrawScene(Head);
	}

	TryPlaySceneSound(Head->Info.Sound);
	PrepareScenes();

	OnSceneStart.Broadcast(*Head);

	return true;
}

bool UVisualController::RequestFastMove(EVisualControllerDirection::Type Direction)
{
	if (IsIdle())
	{
		FastMoveTask = MakeUnique<UE::VisualU::Private::FFastMoveAsyncTask>(this, Direction, bPlayTransitions, bPlaySound);
		bPlayTransitions = false;
		bPlaySound = false;
		FastMoveTask->StartBackgroundTask();

		Mode = EVisualControllerMode::FastMoving;

		OnFastMoveStart.Broadcast(Direction);

		return true;
	}

	return false;
}

bool UVisualController::RequestAutoMove(EVisualControllerDirection::Type Direction)
{
	check(Direction != EVisualControllerDirection::None);

	if (IsIdle())
	{
		Mode = EVisualControllerMode::AutoMoving;

		OnAutoMoveStart.Broadcast(Direction);

		const auto AutoMove = [this, Direction](float DeltaTime) -> bool
		{
			const bool bIsForward = Direction == EVisualControllerDirection::Forward;
			const bool bCanContinue = (bIsForward
				? (!IsWithChoice() && RequestNextScene() && CanAdvanceScene())
				: RequestPreviousScene() && CanRetractScene());

			if (!bCanContinue)
			{
				CancelAutoMove();
			}

			return bCanContinue;
		};

		/*Fire it once first to replicate a do-while style*/
		if (AutoMove(0.f))
		{
			FTickerDelegate TickerDelegate = FTickerDelegate::CreateLambda(AutoMove);
			AutoMoveHandle = FTSTicker::GetCoreTicker().AddTicker(TickerDelegate, AutoMoveDelay);

			return true;
		}
	}

	return false;
}

void UVisualController::CancelFastMove()
{
	if (IsFastMoving() && FastMoveTask.IsValid())
	{
		FastMoveTask->EnsureCompletion(/*bDoWorkOnThisThreadIfNotStarted=*/false);
		FastMoveTask->Cancel();
		FastMoveTask.Reset(nullptr);

		Renderer->DrawScene(GetCurrentScene());

		Mode = EVisualControllerMode::Idle;

		OnFastMoveEnd.Broadcast();
	}
}

void UVisualController::CancelAutoMove()
{
	if (IsAutoMoving())
	{
		if (AutoMoveHandle.IsValid())
		{
			FTSTicker::RemoveTicker(AutoMoveHandle);
		}

		Renderer->DrawScene(GetCurrentScene());

		Mode = EVisualControllerMode::Idle;

		OnAutoMoveEnd.Broadcast();
	}
}

void UVisualController::VisualizeToScreen(TSubclassOf<UVisualRenderer> RendererClass, int32 ZOrder)
{
	check(Renderer);
	if (RendererClass != Renderer->GetClass())
	{
		Renderer = CreateWidget<UVisualRenderer>(GetOuterAPlayerController(), RendererClass);
	}

	Renderer->AddToPlayerScreen(ZOrder);
	const FScenario* CurrentScene = GetCurrentScene();
	TSharedPtr<FStreamableHandle> CurrentSceneHandle = LoadScene(CurrentScene);
	Renderer->DrawScene(CurrentScene);
}

void UVisualController::VisualizeToComponent(TSubclassOf<UVisualRenderer> RendererClass, UWidgetComponent* Component)
{
	check(Renderer);
	check(Component);
	if (RendererClass != Renderer->GetClass())
	{
		Renderer = CreateWidget<UVisualRenderer>(GetOuterAPlayerController(), RendererClass);
	}

	Component->SetWidget(Renderer);
	const FScenario* CurrentScene = GetCurrentScene();
	TSharedPtr<FStreamableHandle> CurrentSceneHandle = LoadScene(CurrentScene);
	Renderer->DrawScene(CurrentScene);
}

void UVisualController::RemoveFromScreen() const
{
	check(Renderer);
	Renderer->RemoveFromParent();
}

void UVisualController::SetRendererVisibility(ESlateVisibility Visibility)
{
	check(Renderer);
	Renderer->SetVisibility(Visibility);
}

ESlateVisibility UVisualController::GetRendererVisibility() const
{
	check(Renderer);
	return Renderer->GetVisibility();
}

void UVisualController::SetNumScenesToLoad(int32 Num)
{
	if (ensureMsgf(Num >= 0, TEXT("Expected positive value, set operation failed.")))
	{
		constexpr int32 ScenesToLoadLargeNum = 100;

		if (Num > ScenesToLoadLargeNum)
		{
			UE_LOG(LogVisualU, Warning, TEXT("Received large (%i) request for async scene loading, it might have a significant impact on performance."), Num);
		}

		ScenesToLoad = Num;
	}
}

void UVisualController::ShouldPlayTransitions(bool bShouldPlay)
{
	bPlayTransitions = bShouldPlay;
}

void UVisualController::ShouldPlaySound(bool bShouldPlay)
{
	bPlaySound = bShouldPlay;
}

void UVisualController::SetAutoMoveDelay(float Delay)
{
	if (ensureMsgf(!FMath::IsNegativeOrNegativeZero(Delay), TEXT("Negative time is invalid, set operation failed.")))
	{
		AutoMoveDelay = Delay;
	}
}

const FScenario* UVisualController::GetCurrentScene() const
{
	return Node[SceneIndex];
}

const FScenario& UVisualController::GetCurrentScenario() const
{
	return *GetCurrentScene();
}

bool UVisualController::CanAdvanceScene() const
{
	return Node.IsValidIndex(SceneIndex + 1);
}

bool UVisualController::CanRetractScene() const
{
	return Node.IsValidIndex(SceneIndex - 1);
}

bool UVisualController::IsWithChoice() const
{
	return GetCurrentScene()->HasChoice();
}

bool UVisualController::IsSceneExhausted(const FScenario* Scene) const
{
	return ExhaustedScenes.Contains(Scene);
}

bool UVisualController::IsScenarioExhausted(const FScenario& Scenario) const
{
	return IsSceneExhausted(&Scenario);
}

bool UVisualController::IsTransitioning() const
{
	check(Renderer);
	return Renderer->IsTransitionInProgress();
}

bool UVisualController::IsCurrentScenarioHead() const
{
	return GetCurrentScene() == Head;
}

const FString UVisualController::GetHeadDebugString() const
{
	FString DebugString;
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	DebugString = Head ? Head->GetDebugString() : TEXT("None");
#endif

	return DebugString;
}

const FString UVisualController::GetAsyncQueueDebugString() const
{
	FString DebugString;
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	for (TWeakPtr<FStreamableHandle> WeakSceneHandle : DebugSceneHandles)
	{
		if (TSharedPtr<FStreamableHandle> SceneHandle = WeakSceneHandle.Pin())
		{
			DebugString += FString::Printf(TEXT("%s Progress: %.2f\n"), *SceneHandle->GetDebugName(), SceneHandle->GetProgress());
		}
	}
#endif

	return DebugString;
}

const FString UVisualController::GetExhaustedScenesDebugString() const
{
	FString DebugString;
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	for (FScenario* const& ExhaustedScene : ExhaustedScenes)
	{
		DebugString += ExhaustedScene ? (ExhaustedScene->GetDebugString() + TEXT("\n")) : TEXT("Invalid\n");
	}
#endif

	return DebugString;
}

TSharedPtr<FStreamableHandle> UVisualController::LoadSceneAsync(const FScenario* Scene, FStreamableDelegate AfterLoadDelegate)
{
	check(Scene);

	TArray<FSoftObjectPath> DataToLoad;

	Scene->GetDataToLoad(DataToLoad);

	FString DebugString = TEXT("ArrayDelegate");
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	DebugString = Scene->GetDebugString();
#endif
	return UAssetManager::GetStreamableManager().RequestAsyncLoad(
		DataToLoad,
		AfterLoadDelegate,
		FStreamableManager::DefaultAsyncLoadPriority,
		/*bManageActiveHandle=*/false,
		/*bStartStalled=*/false,
		DebugString);
}

TSharedPtr<FStreamableHandle> UVisualController::LoadScene(const FScenario* Scene, FStreamableDelegate AfterLoadDelegate)
{
	check(Scene);

	TArray<FSoftObjectPath> DataToLoad;

	Scene->GetDataToLoad(DataToLoad);

	FString DebugString = TEXT("RequestSyncLoad Array");
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	DebugString = Scene->GetDebugString();
#endif
	TSharedPtr<FStreamableHandle> Handle = UAssetManager::GetStreamableManager().RequestSyncLoad(DataToLoad, /*bManageActiveHandle*/false, DebugString);
	
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
				
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
					DebugSceneHandles.PushLast(SceneHandle);
#endif
					SceneHandles.Enqueue(MoveTemp(SceneHandle));
				}
			}

			return;
		}

		const int32 NextIndex = SceneIndex + (Direction == EVisualControllerDirection::Forward ? (ScenesToLoad + 1) : -(ScenesToLoad + 1));
		if (Node.IsValidIndex(NextIndex))
		{
			TSharedPtr<FStreamableHandle> SceneHandle = LoadSceneAsync(GetSceneAt(NextIndex));
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			DebugSceneHandles.PushLast(SceneHandle);
#endif
			SceneHandles.Enqueue(MoveTemp(SceneHandle));
		}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		if (!DebugSceneHandles.IsEmpty())
		{
			DebugSceneHandles.PopFirst();
		}
#endif
		SceneHandles.Dequeue(NextSceneHandle);
	}
}

void UVisualController::TryPlaySceneSound(TSoftObjectPtr<USoundBase> SceneSound) const
{
	if (USoundBase* Sound = SceneSound.Get(); Sound && bPlaySound)
	{
		GetOuterAPlayerController()->ClientPlaySound(Sound);
	}
}

void UVisualController::CancelNextScene()
{
	if (NextSceneHandle.IsValid())
	{
		NextSceneHandle->CancelHandle();
		NextSceneHandle.Reset();
	}
}

bool UVisualController::TryPlayTransition(const FScenario* From, const FScenario* To)
{
	check(Renderer);

	if (bPlayTransitions && !Renderer->IsTransitionInProgress())
	{
		return Renderer->TryDrawTransition(From, To);
	}

	return false;
}

void UVisualController::RollbackTo(const FScenario* Scene)
{
	check(Scene);
	check(Renderer);
	const UDataTable* SceneOwner = Scene->GetOwner();
	const FScenario* CurrentScenario = GetCurrentScene();
	const UDataTable* CurrentSceneOwner = CurrentScenario->GetOwner();
	if (SceneOwner != CurrentSceneOwner)
	{
		NodeReferenceKeeper.Remove(CurrentSceneOwner);
		Node.Empty();
		NodeReferenceKeeper.Add(SceneOwner);
		SceneOwner->GetAllRows(UE_SOURCE_LOCATION, Node);
	}

	OnSceneEnd.Broadcast(*CurrentScenario);
	
	SceneHandles.Empty();
	CancelNextScene();

	SceneIndex = Scene->GetIndex();

	const FScenario* CurrentScene = GetCurrentScene();
	TSharedPtr<FStreamableHandle> CurrentSceneHandle = LoadScene(CurrentScene);
	Renderer->DrawScene(CurrentScene);
	PrepareScenes(EVisualControllerDirection::Backward);

	OnSceneStart.Broadcast(*CurrentScene);
}

UVisualVersioningSubsystem* UVisualController::TryGetVisualVersioningSubsystem() const
{
	if (ULocalPlayer* LocalPlayer = GetOuterAPlayerController()->GetLocalPlayer())
	{
		return LocalPlayer->GetSubsystem<UVisualVersioningSubsystem>();
	}

	return nullptr;
}

void UVisualController::AssertNextSceneLoad(EVisualControllerDirection::Type Direction)
{
	check(Direction != EVisualControllerDirection::None);
	const int32 NextSceneIndex = SceneIndex + StaticCast<int32>(Direction);
	NextSceneHandle = LoadScene(GetSceneAt(NextSceneIndex));
}
