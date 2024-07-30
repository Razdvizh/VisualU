// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualController.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Engine/DataTable.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Misc/App.h"
#include "Tasks/Task.h"
#include "GameFramework/PlayerController.h"
#include "Sound/SoundBase.h"
#include "VisualVersioningSubsystem.h"
#include "VisualUCustomVersion.h"
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
	ScenesToLoad(5),
	SceneHandles(),
	ExhaustedScenes(),
	Head(nullptr),
	FastMoveTask(nullptr),
	AutoMoveHandle(nullptr),
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
	}

	Super::BeginDestroy();
}

void UVisualController::Experimental_SerializeController(FArchive& Ar)
{
	Ar.UsingCustomVersion(FVisualUCustomVersion::GUID);
	Ar.ArIsSaveGame = true;

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
		int32 NumExhaustedScenes;
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
		CurrentScenario.GetOwner()->GetAllRows(UE_SOURCE_LOCATION, Node);
		SceneIndex = CurrentScenario.GetIndex();

		FScenario SavedHead;
		Ar << SavedHead;
		Head = FScenario::ResolveScene(SavedHead);
	}

	Super::Serialize(Ar);
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

	if (GEngine)
	{
		bPlaySound &= GEngine->UseSound();
	}

	if (IsInGameThread() && FApp::IsGame())
	{
		const UVisualUSettings* VisualUSettings = GetDefault<UVisualUSettings>();

		checkf(!VisualUSettings->FirstDataTable.IsNull(), TEXT("Unable to find first data table, please specify one in project settings."));
		const UDataTable* FirstDataTable = VisualUSettings->FirstDataTable.LoadSynchronous();

		checkf(FirstDataTable->GetRowStruct()->IsChildOf(FScenario::StaticStruct()), TEXT("Data table must be based on FScenario struct."));
		FirstDataTable->GetAllRows(UE_SOURCE_LOCATION, Node);

		checkf(Node.IsValidIndex(0), TEXT("First Data Table is empty!"));
		Head = GetCurrentScene();
	}
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

void UVisualController::TryPlaySceneSound(TSoftObjectPtr<USoundBase> SceneSound) const
{
	if (bPlaySound)
	{
		if (USoundBase* Sound = SceneSound.Get())
		{
			GetOuterAPlayerController()->ClientPlaySound(Sound);
		}
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
			FScenario* Scene = ExhaustedScenes.Pop();
			if (UVisualVersioningSubsystem* VisualVersioning = GetOuterAPlayerController()->GetLocalPlayer()->GetSubsystem<UVisualVersioningSubsystem>())
			{
				VisualVersioning->Checkout(Scene);
			}
			SetCurrentScene(Scene);
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
	if (ensureMsgf(!(Scene->GetOwner() == Head->GetOwner() && Scene->GetIndex() > Head->GetIndex()), TEXT("Only \"seen\" scene can be requested - %s"), *Scene->GetDebugString()))
	{
		return false;
	}

	bool bIsFound = false;
	if (Node[0]->GetOwner() == Scene->GetOwner())
	{
		bIsFound = true;
	}
	else
	{
		UVisualVersioningSubsystem* VisualVersioning = nullptr;
		if (ULocalPlayer* LocalPlayer = GetOuterAPlayerController()->GetLocalPlayer())
		{
			VisualVersioning = LocalPlayer->GetSubsystem<UVisualVersioningSubsystem>();
		}
		/*Traverse the stack until the requested Node is found*/
		for (int32 i = ExhaustedScenes.Num() - 1; i >= 0; i--)
		{
			if (ExhaustedScenes[i]->GetOwner() == Scene->GetOwner())
			{
				FScenario* ExhaustedScene = ExhaustedScenes.Pop();
				if (VisualVersioning)
				{
					VisualVersioning->Checkout(ExhaustedScene);
				}
				bIsFound = true;
				break;
			}
			FScenario* ExhaustedScene = ExhaustedScenes.Pop();
			if (VisualVersioning)
			{
				VisualVersioning->Checkout(ExhaustedScene);
			}
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
	if (Scene->GetOwner() != GetSceneAt(0)->GetOwner())
	{
		Node.Empty();
		Scene->GetOwner()->GetAllRows(UE_SOURCE_LOCATION, Node);
	}

	OnSceneEnd.Broadcast();
	OnNativeSceneEnd.Broadcast();
	
	SceneHandles.Empty();
	CancelNextScene();

	SceneIndex = Scene->GetIndex();
	TSharedPtr<FStreamableHandle> CurrentSceneHandle = LoadScene(GetCurrentScene());
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
	if (Renderer->IsTransitionInProgress() || IsFastMoving() || IsAutoMoving())
	{
		return false;
	}
	check(NewNode);
	checkf(!NewNode->GetRowMap().IsEmpty(), TEXT("Jumping to empty node is not allowed."));
	checkf(GetCurrentScene()->GetOwner() != NewNode, TEXT("Jumping to active node is not allowed."));
	checkf(NewNode->GetRowStruct()->IsChildOf(FScenario::StaticStruct()), TEXT("Node must be based on FScenario struct."));
#if !UE_BUILD_SHIPPING
	for (const FScenario* ExhaustedScene : ExhaustedScenes)
	{
		checkf(ExhaustedScene->GetOwner() != NewNode, TEXT("Jumping to already \"seen\" nodes is invalid. Use RequestScene or RequestPreviousScene instead."));
	}
#endif
	
	FScenario* Last = Node[SceneIndex];
	const UDataTable* LastNode = Last->GetOwner();
	ExhaustedScenes.Push(Last);

	Node.Empty();
	NewNode->GetAllRows(UE_SOURCE_LOCATION, Node);

	checkf(!Node.IsEmpty(), TEXT("Trying to jump to empty Data Table! - %s"), *NewNode->GetFName().ToString());

	OnSceneEnd.Broadcast();
	OnNativeSceneEnd.Broadcast();

	SceneHandles.Empty();
	CancelNextScene();

	SceneIndex = 0;

	const FScenario* CurrentScene = GetCurrentScene();
	if (Head->GetOwner() == LastNode)
	{
		Head = CurrentScene;
	}
	TSharedPtr<FStreamableHandle> CurrentSceneHandle = LoadScene(CurrentScene);
	if (!TryPlayTransition(Last, CurrentScene))
	{
		Renderer->DrawScene(CurrentScene);
	}

	TryPlaySceneSound(CurrentScene->Info.Sound);
	PrepareScenes();

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();

	return true;
}

void UVisualController::RequestFastMove(EVisualControllerDirection::Type Direction)
{
	if (IsIdle())
	{
		FastMoveTask = MakeUnique<UE::VisualU::Private::FFastMoveAsyncTask>(this, Direction, bPlayTransitions, bPlaySound);
		bPlayTransitions = false;
		bPlaySound = false;
		Mode = EVisualControllerMode::FastMoving;
		FastMoveTask->StartBackgroundTask();
	}
}

void UVisualController::RequestAutoMove(EVisualControllerDirection::Type Direction)
{
	if (IsIdle())
	{
		check(Direction != EVisualControllerDirection::None);
		Mode = EVisualControllerMode::AutoMoving;
		const auto AutoMove = [this, Direction](float DeltaTime) -> bool
		{
			const bool bCanContinue = (Direction == EVisualControllerDirection::Forward
				? (!IsWithChoice() && RequestNextScene())
				: RequestPreviousScene());

			if (!bCanContinue)
			{
				CancelAutoMove();
			}

			return bCanContinue;
		};

		/*Fire it once first to replicate a do-while style*/
		FTSTicker::FDelegateHandle Handle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda(AutoMove), 0.f);
		FTSTicker::RemoveTicker(Handle);
		AutoMoveHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda(AutoMove), AutoMoveDelay);
	}
}

void UVisualController::CancelFastMove()
{
	if (IsFastMoving() && FastMoveTask.IsValid())
	{
		FastMoveTask->EnsureCompletion(/*bDoWorkOnThisThreadIfNotStarted=*/false);
		FastMoveTask->Cancel();
		FastMoveTask.Reset(nullptr);

		Mode = EVisualControllerMode::Idle;
	}
}

void UVisualController::CancelAutoMove()
{
	if (IsAutoMoving() && AutoMoveHandle.IsValid())
	{
		FTSTicker::RemoveTicker(AutoMoveHandle);
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

	const FScenario* CurrentScene = GetCurrentScene();
	TSharedPtr<FStreamableHandle> CurrentSceneHandle = LoadScene(CurrentScene);
	Renderer->AddToPlayerScreen(ZOrder);
	Renderer->DrawScene(CurrentScene);
	TryPlaySceneSound(CurrentScene->Info.Sound);
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
