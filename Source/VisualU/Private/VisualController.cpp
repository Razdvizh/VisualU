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

		AssertCurrentSceneLoad();
		return;
	}

	const int32 NextIndex = SceneIndex + (bIsForward ? (ScenesToLoad - 1) : -(ScenesToLoad - 1));
	if (Node.IsValidIndex(NextIndex))
	{
		TSharedPtr<FStreamableHandle> SceneHandle = LoadSceneAsync(GetSceneAt(NextIndex));
		SceneHandles.Enqueue(MoveTemp(SceneHandle));
	}

	SceneHandles.Dequeue(ActiveSceneHandle);

	AssertCurrentSceneLoad(bIsForward);
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

	//Renderer->PlayTransition(GetCurrentScene(), GetSceneAt(SceneIndex + 1));
	SceneIndex += 1;
	PrepareScenes();
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
	Renderer->DrawScene(GetCurrentScene());

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();
}

bool UVisualController::ToScene(const FScenario* Scene)
{
	check(Scene);
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
		SceneHandles.Empty();
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
	OnSceneEnd.Broadcast();
	OnNativeSceneEnd.Broadcast();

	if (Scene->Owner != GetSceneAt(0)->Owner)
	{
		Node.Empty();
		SceneHandles.Empty();
		Scene->Owner->GetAllRows(UE_SOURCE_LOCATION, Node);
	}

	SceneIndex = Scene->Index;
	PrepareScenes();
	Renderer->DrawScene(GetCurrentScene());

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();
}

void UVisualController::AssertCurrentSceneLoad(bool bIsForward)
{
	if (SceneHandles.IsEmpty() || SceneHandles.Peek()->Get()->IsLoadingInProgress())
	{
		LoadScene(GetCurrentScene());
	}
	else if ((bIsForward && !CanAdvanceScene()) || (!bIsForward && !CanRetractScene()))
	{
		SceneHandles.Dequeue(ActiveSceneHandle);
	}
}

void UVisualController::ToNode(const UDataTable* NewNode)
{
	check(NewNode);
	ExhaustedScenes.Push(Node.Last());

	Node.Empty();
	NewNode->GetAllRows(UE_SOURCE_LOCATION, Node);

	checkf(!Node.IsEmpty(), TEXT("Trying to jump to empty Data Table! - %s"), *NewNode->GetFName().ToString());

	OnSceneEnd.Broadcast();
	OnNativeSceneEnd.Broadcast();

	SceneHandles.Empty();

	SceneIndex = 0;
	PrepareScenes();
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

void UVisualController::SetNumScenesToLoad(int32 Num)
{
	if (Num > 100)
	{
		UE_LOG(LogVisualU, Warning, TEXT("Received large (%i) request for scenario loading, it might have a significant impact on performance."), Num);
	}

	ScenesToLoad = Num;
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
