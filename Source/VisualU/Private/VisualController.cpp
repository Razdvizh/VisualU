// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualController.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Engine/DataTable.h"
#include "GameFramework/PlayerController.h"
#include "VisualUSettings.h"
#include "VisualRenderer.h"

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
static TAutoConsoleVariable<bool> CVarDebugDeferredSceneLoading
(
	TEXT("VisualU.DebugDefferedSceneLoading"),
	false,
	TEXT("Displays information about asset loading of future FScenarios"),
	ECVF_Cheat
);
#endif

UVisualController::UVisualController() 
	: Super(),
	RendererClass(UVisualRenderer::StaticClass()),
	Renderer(nullptr),
	ActiveSceneHandle(nullptr),
	Node(),
	SceneIndex(0),
	ScenesToLoad(5),
	SceneHandles(),
	ExhaustedScenes()
{
	const UVisualUSettings* VisualUSettings = GetDefault<UVisualUSettings>();

	check(!VisualUSettings->FirstDataTable.IsNull());
	const UDataTable* FirstDataTable = VisualUSettings->FirstDataTable.LoadSynchronous();

	check(FirstDataTable->GetRowStruct()->IsChildOf(FScenario::StaticStruct()));
	FirstDataTable->GetAllRows(UE_SOURCE_LOCATION, Node);

	checkf(Node.IsValidIndex(0), TEXT("First Data Table is empty!"));
}

TSharedPtr<FStreamableHandle> UVisualController::LoadSceneAsync(const FScenario* Scene)
{
	check(Scene);

	TArray<FSoftObjectPath> DataToLoad;

	Scene->GetDataToLoad(DataToLoad);

	TWeakObjectPtr<UVisualController> WeakThis = TWeakObjectPtr<UVisualController>(this);
	return UAssetManager::GetStreamableManager().RequestAsyncLoad(DataToLoad, [WeakThis]()
	{
		if (UVisualController* Pinned = WeakThis.Get())
		{
			Pinned->OnSceneLoaded.Broadcast();
			Pinned->OnNativeSceneLoaded.Broadcast();
		}
	}, FStreamableManager::DefaultAsyncLoadPriority);
}

void UVisualController::LoadScene(const FScenario* Scene)
{
	check(Scene);

	TArray<FSoftObjectPath> DataToLoad;

	Scene->GetDataToLoad(DataToLoad);

	ActiveSceneHandle = UAssetManager::GetStreamableManager().RequestSyncLoad(DataToLoad, false);

	OnSceneLoaded.Broadcast();
	OnNativeSceneLoaded.Broadcast();
}

void UVisualController::PrepareScenes(bool bIsForward)
{
	if (SceneHandles.IsEmpty())
	{
		for (int32 i = 1; i <= ScenesToLoad; i++)
		{
			const int32 u = bIsForward ? i : -i;
			if (Node.IsValidIndex(SceneIndex + u))
			{
				const FScenario* Scene = GetSceneAt(SceneIndex + u);
				TSharedPtr<FStreamableHandle> SceneHandle = LoadSceneAsync(Scene);

				SceneHandles.Enqueue(SceneHandle);
			}
		}

		return;
	}

	const int32 NextIndex = SceneIndex + (bIsForward ? (ScenesToLoad - 1) : -(ScenesToLoad - 1));
	if (Node.IsValidIndex(NextIndex))
	{
		TSharedPtr<FStreamableHandle> SceneHandle = LoadSceneAsync(GetSceneAt(NextIndex));
		SceneHandles.Enqueue(MoveTemp(SceneHandle));
	}

	SceneHandles.Dequeue(ActiveSceneHandle);
}

void UVisualController::ToNextScene()
{
	if (!CanAdvanceScene())
	{
		return;
	}

	OnSceneEnd.Broadcast();
	OnNativeSceneEnd.Broadcast();

	CancelCurrentScene();

	SceneIndex += 1;
	PrepareScenes();
	FallbackQueue();
	Renderer->DrawScene(GetCurrentScene());

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

	SceneIndex -= 1;
	PrepareScenes(false);
	FallbackQueue(false);
	Renderer->DrawScene(GetCurrentScene());

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();
}

bool UVisualController::ToScene(const FScenario* Scene)
{
	bool bIsFound = false;
	if (Node[0]->Owner == Scene->Owner)
	{
		bIsFound = true;
	}
	else
	{
		/*Traverse the stack until the requested Node is found*/
		for (int i = ExhaustedScenes.Num() - 1; i >= 0; i--)
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
	OnSceneEnd.Broadcast();
	OnNativeSceneEnd.Broadcast();

	if (Scene->Owner != GetSceneAt(0)->Owner)
	{
		Node.Empty();
		Scene->Owner->GetAllRows(UE_SOURCE_LOCATION, Node);
	}

	SceneIndex = Scene->Index;
	Renderer->DrawScene(GetCurrentScene());

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();
}

void UVisualController::FallbackQueue(bool bIsForward)
{
	if (SceneHandles.IsEmpty() || SceneHandles.Peek()->Get()->IsLoadingInProgress())
	{
		LoadScene(GetCurrentScene());
	}
	if ((bIsForward && !CanAdvanceScene()) || (!bIsForward && !CanRetractScene()))
	{
		SceneHandles.Dequeue(ActiveSceneHandle);
	}
}

void UVisualController::ToNode(const UDataTable* NewNode)
{
	ExhaustedScenes.Push(Node.Last());

	Node.Empty();
	NewNode->GetAllRows(UE_SOURCE_LOCATION, Node);

	OnSceneEnd.Broadcast();
	OnNativeSceneEnd.Broadcast();

	SceneIndex = 0;
	Renderer->DrawScene(GetCurrentScene());

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();
}

void UVisualController::Visualize(APlayerController* OwningController, int32 ZOrder)
{
	if (!IsValid(Renderer))
	{
		checkf(OwningController, TEXT("Supplied PlayerController is invalid."));
		Renderer = CreateWidget<UVisualRenderer>(OwningController, RendererClass);
	}
	Renderer->AddToPlayerScreen(ZOrder);
	LoadScene(GetCurrentScene());
	PrepareScenes();
	Renderer->DrawScene(GetCurrentScene());
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

bool UVisualController::IsWithChoice() const
{
	return GetCurrentScene()->HasChoice();
}

void UVisualController::CancelCurrentScene()
{
	if (ActiveSceneHandle.IsValid())
	{
		ActiveSceneHandle->CancelHandle();
		ActiveSceneHandle.Reset();
	}
}

bool UVisualController::IsCurrentSceneLoading() const
{
	if (ActiveSceneHandle.IsValid())
	{
		return ActiveSceneHandle->IsLoadingInProgress();
	}

	return false;
}

bool UVisualController::IsCurrentSceneLoaded() const
{
	if (ActiveSceneHandle.IsValid())
	{
		return ActiveSceneHandle->HasLoadCompleted();
	}

	return false;
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
