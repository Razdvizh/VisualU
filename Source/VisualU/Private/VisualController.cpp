// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualController.h"

AVisualController::AVisualController() 
	: Super()
{
}

void AVisualController::BeginPlay()
{
	Super::BeginPlay();

	if (bDrawOnBeginPlay) {}
}

void AVisualController::LoadScene(const FScenario* Scene, FStreamableDelegate AfterLoadDelegate, bool UnloadScene)
{
	check(Scene);
	if (UnloadScene)
	{
		CancelSceneLoading();
	}

	TArray<FSoftObjectPath> DataToLoad;

	Scene->GetDataToLoad(DataToLoad);

	TWeakObjectPtr<AVisualController> WeakThis = TWeakObjectPtr<AVisualController>(this);
	ActiveSceneHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(DataToLoad, [WeakThis, AfterLoadDelegate]()
	{
		if (AVisualController* Pinned = WeakThis.Get())
		{
			AfterLoadDelegate.ExecuteIfBound();
			Pinned->OnSceneLoaded.Broadcast();
			Pinned->OnNativeSceneLoaded.Broadcast();
		}
	}, FStreamableManager::DefaultAsyncLoadPriority);
}

void AVisualController::ToNextScene()
{
	if (!CanAdvanceScene())
	{
		return;
	}

	OnSceneEnd.Broadcast();
	OnNativeSceneEnd.Broadcast();

	SceneIndex += 1;
	//LoadAndConstruct();

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();
}

void AVisualController::ToPreviousScene()
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
//	LoadAndConstruct();

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();
}

bool AVisualController::ToScene(const FScenario* Scene)
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

const FScenario* AVisualController::GetSceneAt(int32 Index)
{
	check(Node.IsValidIndex(Index));
	return Node[Index];
}

bool AVisualController::ToScenario(const FScenario& Scenario)
{
	return ToScene(&Scenario);
}

void AVisualController::SetCurrentScene(const FScenario* Scene)
{
	OnSceneEnd.Broadcast();
	OnNativeSceneEnd.Broadcast();

	if (Scene->Owner != GetSceneAt(0)->Owner)
	{
		Node.Empty();
		Scene->Owner->GetAllRows(UE_SOURCE_LOCATION, Node);
	}

	SceneIndex = Scene->Index;
	//LoadAndConstruct();

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();
}

void AVisualController::ToNode(const UDataTable* NewNode)
{
	ExhaustedScenes.Push(Node.Last());

	Node.Empty();
	NewNode->GetAllRows(UE_SOURCE_LOCATION, Node);

	OnSceneEnd.Broadcast();
	OnNativeSceneEnd.Broadcast();

	SceneIndex = 0;
	//LoadAndConstruct();

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();
}

void AVisualController::CancelSceneLoading()
{
	if (ActiveSceneHandle.IsValid())
	{
		ActiveSceneHandle->CancelHandle();
		ActiveSceneHandle.Reset();
	}
}

bool AVisualController::IsWithChoice() const
{
	return GetCurrentScene()->HasChoice();
}

bool AVisualController::IsSceneLoading() const
{
	if (ActiveSceneHandle.IsValid())
	{
		return ActiveSceneHandle->IsLoadingInProgress();
	}

	return false;
}

bool AVisualController::IsSceneLoaded() const
{
	if (ActiveSceneHandle.IsValid())
	{
		return ActiveSceneHandle->HasLoadCompleted();
	}

	return false;
}

bool AVisualController::CanAdvanceScene() const
{
	return Node.IsValidIndex(SceneIndex + 1);
}

bool AVisualController::CanRetractScene() const
{
	return Node.IsValidIndex(SceneIndex - 1);
}

bool AVisualController::IsSceneExhausted(const FScenario* Scene) const
{
	return ExhaustedScenes.Contains(Scene);
}

bool AVisualController::IsScenarioExhausted(const FScenario& Scenario) const
{
	return IsSceneExhausted(&Scenario);
}

const FScenario* AVisualController::GetCurrentScene() const
{
	return Node[SceneIndex];
}

const FScenario& AVisualController::GetCurrentScenario() const
{
	return *GetCurrentScene();
}