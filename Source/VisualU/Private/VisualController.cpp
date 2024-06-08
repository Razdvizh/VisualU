// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualController.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Engine/DataTable.h"
#include "GameFramework/PlayerController.h"
#include "VisualUSettings.h"
#include "VisualRenderer.h"

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
static TAutoConsoleVariable<float> CVarEditorStallThreadForLoading
(
	TEXT("VisualU.EditorStallThreadForLoading"),
	0.f,
	TEXT("Editor only. Has effect when > 0.f. Sleeps process for specified amount of time during next scene loading to give GC time to catch up."),
	ECVF_Cheat
);
#endif

UVisualController::UVisualController() 
	: Super(),
	Renderer(nullptr),
	NextSceneHandle(nullptr),
	Node(),
	SceneIndex(0),
	ScenesToLoad(5),
	SceneHandles(),
	ExhaustedScenes(),
	bPlayTransitions(true)
{
	const UVisualUSettings* VisualUSettings = GetDefault<UVisualUSettings>();

	check(!VisualUSettings->FirstDataTable.IsNull());
	const UDataTable* FirstDataTable = VisualUSettings->FirstDataTable.LoadSynchronous();

	check(FirstDataTable->GetRowStruct()->IsChildOf(FScenario::StaticStruct()));
	FirstDataTable->GetAllRows(UE_SOURCE_LOCATION, Node);

	checkf(Node.IsValidIndex(0), TEXT("First Data Table is empty!"));
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

void UVisualController::PrepareScenes(ENodeDirection Direction)
{
	if (ScenesToLoad > 0)
	{
		if (SceneHandles.IsEmpty())
		{
			for (int32 i = 1; i <= ScenesToLoad; i++)
			{
				const int32 u = Direction == ENodeDirection::Forward ? i : -i;
				if (Node.IsValidIndex(SceneIndex + u))
				{
					const FScenario* Scene = GetSceneAt(SceneIndex + u);
					TSharedPtr<FStreamableHandle> SceneHandle = LoadSceneAsync(Scene);
					
					SceneHandles.Enqueue(SceneHandle);
				}
			}

			return;
		}

		const int32 NextIndex = SceneIndex + (Direction == ENodeDirection::Forward ? (ScenesToLoad + 1) : -(ScenesToLoad + 1));
		if (Node.IsValidIndex(NextIndex))
		{
			TSharedPtr<FStreamableHandle> SceneHandle = LoadSceneAsync(GetSceneAt(NextIndex));
			SceneHandles.Enqueue(MoveTemp(SceneHandle));
		}

		SceneHandles.Dequeue(NextSceneHandle);
	}
}

void UVisualController::ToNextScene()
{
	if (!CanAdvanceScene() || Renderer->IsTransitionInProgress())
	{
		return;
	}
	
	OnSceneEnd.Broadcast();
	OnNativeSceneEnd.Broadcast();

	PrepareScenes();
	AssertNextSceneLoad();

	SceneIndex += 1;

	if (!TryPlayTransition(GetSceneAt(SceneIndex - 1), GetCurrentScene()))
	{
		Renderer->DrawScene(GetCurrentScene());
	}

	CancelNextScene();
	
	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();
}

void UVisualController::ToPreviousScene()
{
	if (!CanRetractScene())
	{
		if (!ExhaustedScenes.IsEmpty())
		{
			SetCurrentScene(ExhaustedScenes.Pop());
		}
		return;
	}

	OnSceneEnd.Broadcast();
	OnNativeSceneEnd.Broadcast();

	PrepareScenes(ENodeDirection::Backward);
	AssertNextSceneLoad(ENodeDirection::Backward);

	SceneIndex -= 1;
	Renderer->DrawScene(GetCurrentScene());
	
	CancelNextScene();

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();
}

bool UVisualController::ToScene(const FScenario* Scene)
{
	check(Scene);
	if (Renderer->IsTransitionInProgress())
	{
		return false;
	}
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

bool UVisualController::ToScenario(const FScenario& Scenario)
{
	return ToScene(&Scenario);
}

void UVisualController::SetCurrentScene(const FScenario* Scene)
{
	check(Scene);
	if (Renderer->IsTransitionInProgress())
	{
		return;
	}

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

void UVisualController::AssertNextSceneLoad(ENodeDirection Direction)
{
	const int32 NextSceneIndex = SceneIndex + (Direction == ENodeDirection::Forward ? 1 : -1);
	NextSceneHandle = LoadScene(GetSceneAt(NextSceneIndex));

	#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST) && WITH_EDITOR
	FPlatformProcess::Sleep(CVarEditorStallThreadForLoading.GetValueOnAnyThread());
	#endif
}

void UVisualController::ToNode(const UDataTable* NewNode)
{
	check(NewNode);
	if (Renderer->IsTransitionInProgress())
	{
		return;
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
	LoadScene(GetCurrentScene());
	if (!TryPlayTransition(Last, GetCurrentScene()))
	{
		Renderer->DrawScene(GetCurrentScene());
	}
	PrepareScenes();

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();
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

void UVisualController::Visualize(APlayerController* OwningController, const TSubclassOf<UVisualRenderer>& RendererClass, int32 ZOrder)
{
	if (!IsValid(Renderer))
	{
		checkf(OwningController, TEXT("Supplied PlayerController is invalid."));
		Renderer = CreateWidget<UVisualRenderer>(OwningController, RendererClass);
	}

	LoadScene(GetCurrentScene());
	Renderer->AddToPlayerScreen(ZOrder);
	Renderer->DrawScene(GetCurrentScene());
	PrepareScenes();
}

void UVisualController::Discard()
{
	Renderer->RemoveFromParent();
}

void UVisualController::Show()
{
	check(Renderer);
	Renderer->SetVisibility(ESlateVisibility::Visible);
}

void UVisualController::Hide()
{
	check(Renderer);
	Renderer->SetVisibility(ESlateVisibility::Collapsed);
}

void UVisualController::SetNumScenesToLoad(int32 Num)
{
	if (Num > 100)
	{
		UE_LOG(LogVisualU, Warning, TEXT("Received large (%i) request for scene loading, it might have a significant impact on performance."), Num);
	}

	ScenesToLoad = Num;
}

void UVisualController::SetPlayTransitions(bool bShouldPlay)
{
	bPlayTransitions = bShouldPlay;
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
