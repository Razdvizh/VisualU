// Fill out your copyright notice in the Description page of Project Settings.

#include "VisualScene.h"
#include "Engine/StreamableManager.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Animation/UMGSequencePlayer.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Scenario.h"
#include "VisualDefaults.h"
#include "VisualSprite.h"
#include "VisualImage.h"
#include "BackgroundVisualImage.h"
#include "Materials/MaterialInterface.h"
#include "TransitionMaterialProxy.h"
#include "TimerManager.h"
#include "VisualUSettings.h"
#include "Sound/SoundCue.h"
#include "PaperFlipbook.h"

//keep this below zero to avoid artifacts
constexpr float TRANSITION_THRESHOLD = -0.005f;

UVisualScene* UVisualScene::Get()
{
	for (TObjectIterator<UVisualScene> Itr; Itr; ++Itr)
	{
		if (Itr->IsInViewport())
		{
			return *Itr;
		}
	}
	
	return nullptr;
}

UVisualScene::UVisualScene(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer),
	Transition(nullptr),
	Background(nullptr),
	Canvas(nullptr),
	SceneIndex(0)
{
	OnNativeSceneTransitionEnded.AddUFunction(this, TEXT("ToNextScene"));
}

TSharedRef<SWidget> UVisualScene::RebuildWidget()
{
	Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Canvas"));
	WidgetTree->RootWidget = Canvas;

	TSharedRef<SWidget> VisualSceneSlate = Super::RebuildWidget();

	Background = WidgetTree->ConstructWidget<UBackgroundVisualImage>(UBackgroundVisualImage::StaticClass(), TEXT("Background"));
	UCanvasPanelSlot* const BackgroundSlot = Canvas->AddChildToCanvas(Background);
	check(BackgroundSlot);
	BackgroundSlot->SetAnchors(FVisualAnchors::FullScreen);
	BackgroundSlot->SetOffsets(FVisualMargin::Zero);
	BackgroundSlot->SetZOrder(INT_MIN);

	return VisualSceneSlate;
}

void UVisualScene::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	const UVisualUSettings* VisualUSettings = GetDefault<UVisualUSettings>();
	const UDataTable* FirstDataTable = VisualUSettings->FirstDataTable.LoadSynchronous();
	check(FirstDataTable);
	check(FirstDataTable->GetRowStruct()->IsChildOf(FScenario::StaticStruct()));
	FirstDataTable->GetAllRows(TEXT("VisualScene.cpp(74)"), Node);
	checkf(Node.IsValidIndex(0), TEXT("First Data Table is empty!"));
}

void UVisualScene::NativeConstruct()
{
	Super::NativeConstruct();

	LoadAndConstruct();

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();
}

void UVisualScene::ConstructScene(const FScenario* Scene)
{
	check(Scene);

	ClearSprites();

	if (UPaperFlipbook* BackgroundArt = Scene->Background.BackgroundArt.Get())
	{
		Background->SetFlipbook(BackgroundArt);
	}

	if (USoundBase* Music = Scene->Music.Get())
	{
		PlaySound(Music);
	}

	for (const auto& SpriteData : Scene->SpritesParams)
	{
		UVisualSprite* Sprite = WidgetTree->ConstructWidget<UVisualSprite>(SpriteData.SpriteClass.Get(), SpriteData.SpriteClass->GetFName());
		Sprite->AssignSpriteInfo(SpriteData.SpriteInfo);
		
		UCanvasPanelSlot* SpriteSlot = Canvas->AddChildToCanvas(Sprite);
		SpriteSlot->SetZOrder(SpriteData.ZOrder);
		SpriteSlot->SetAnchors(SpriteData.Anchors);
		SpriteSlot->SetAutoSize(true);
		SpriteSlot->SetPosition(SpriteData.Position);
	}
}

void UVisualScene::LoadScene(const FScenario* Scene, FStreamableDelegate AfterLoadDelegate, bool UnloadScene)
{
	check(Scene);
	if (UnloadScene) CancelSceneLoading();
	
	TArray<FSoftObjectPath> DataToLoad;
	
	Scene->GetDataToLoad(DataToLoad);

	TWeakObjectPtr<UVisualScene> WeakThis = TWeakObjectPtr<UVisualScene>(this);
	ActiveSceneHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(DataToLoad, [WeakThis, AfterLoadDelegate]()
	{
		if (UVisualScene* Pinned = WeakThis.Get())
		{
			AfterLoadDelegate.ExecuteIfBound();
			Pinned->OnSceneLoaded.Broadcast();
			Pinned->OnNativeSceneLoaded.Broadcast();
		}
	}, FStreamableManager::DefaultAsyncLoadPriority);
}

void UVisualScene::LoadAndConstruct()
{
	FStreamableDelegate ConstructSceneDelegate{};
	const FScenario* Scene = Node[SceneIndex];
	TWeakObjectPtr<UVisualScene> WeakThis = TWeakObjectPtr<UVisualScene>(this);
	ConstructSceneDelegate.BindLambda([WeakThis, Scene]()
	{
		if (UVisualScene* Pinned = WeakThis.Get())
		{
			Pinned->ConstructScene(Scene);
		}
	});
	LoadScene(Scene, ConstructSceneDelegate);
}

void UVisualScene::ToNextScene()
{
	if (!CanAdvanceScene())
	{
		return;
	}

	OnSceneEnd.Broadcast();
	OnNativeSceneEnd.Broadcast();

	SceneIndex += 1;
	LoadAndConstruct();

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();
}

void UVisualScene::TransitionToNextScene()
{
	if (ensureAlwaysMsgf(Transition, TEXT("Transition-driving animation is not found. Animation name should be \"Transition\" ")))
	{
		TransitionToNextScene(Transition);
	}
}

void UVisualScene::TransitionToNextScene(UWidgetAnimation* DrivingAnim)
{
	OnSceneTransitionStarted.Broadcast();
	OnNativeSceneTransitionStarted.Broadcast();

	const int32 NumChildren = Canvas->GetChildrenCount();
	for (int32 i = 1; i < NumChildren; i++)
	{
		if (UVisualSprite* Sprite = Cast<UVisualSprite>(Canvas->GetChildAt(i)))
		{
			Sprite->OnSpriteBeginRemove.Broadcast();
		}
	}

	if (!Background->IsTransitioning())
	{
		PlayTransition(DrivingAnim);
	}
}

void UVisualScene::ToPreviousScene()
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
	LoadAndConstruct();

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();
}

bool UVisualScene::ToScene(const FScenario* Scene)
{
	bool isFound = false;
	if (Node[0]->Owner == Scene->Owner)
	{
		isFound = true;
	}
	else
	{
		/*Traverse the stack until the requested Node is found*/
		for (int i = ExhaustedScenes.Num() - 1; i >= 0; i--)
		{
			if (ExhaustedScenes[i]->Owner == Scene->Owner)
			{
				ExhaustedScenes.Pop();
				isFound = true;
				break;
			}
			ExhaustedScenes.Pop();
		}
	}
	
	if (isFound)
	{
		SetCurrentScene(Scene);
	}

	return isFound;
}

const FScenario* UVisualScene::GetSceneAt(int32 Index)
{
	check(Node.IsValidIndex(Index));
	return Node[Index];
}

bool UVisualScene::ToScenario(const FScenario& Scenario)
{
	return ToScene(&Scenario);
}

void UVisualScene::SetCurrentScene(const FScenario* Scene)
{
	OnSceneEnd.Broadcast();
	OnNativeSceneEnd.Broadcast();
	
	if (Scene->Owner != GetSceneAt(0)->Owner)
	{
		Node.Empty();
		Scene->Owner->GetAllRows(TEXT("VisualScene.cpp(266)"), Node);
	}

	SceneIndex = Scene->Index;
	LoadAndConstruct();

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();
}

void UVisualScene::ToNode(const UDataTable* NewNode)
{
	ExhaustedScenes.Push(Node.Last());

	Node.Empty();
	NewNode->GetAllRows(TEXT("VisualScene.cpp(281)"), Node);

	OnSceneEnd.Broadcast();
	OnNativeSceneEnd.Broadcast();

	SceneIndex = 0;
	LoadAndConstruct();

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();
}

void UVisualScene::PlayTransition(UWidgetAnimation* DrivingAnim)
{
	checkf(DrivingAnim, TEXT("Attempted to start transition with invalid animation"));
	const FScenario* CurrentScene = GetCurrentScene();
	const TSoftObjectPtr<UMaterialInterface> SoftTransitionMaterial = CurrentScene->Background.TransitionMaterial;
	if (!SoftTransitionMaterial.IsNull() && CanAdvanceScene())
	{
		const FScenario* NextScene = GetSceneAt(SceneIndex + 1);
		bool bIsTransitionPossible = ensureMsgf(!NextScene->Background.BackgroundArt.IsNull(), TEXT("You are trying to transition to an empty background"));
		if (bIsTransitionPossible)
		{
			UPaperFlipbook* NextFlipbook = NextScene->Background.BackgroundArt.LoadSynchronous();
			UMaterialInterface* TransitionMaterial = SoftTransitionMaterial.IsValid() ? SoftTransitionMaterial.Get() : SoftTransitionMaterial.LoadSynchronous();
			UMaterialInstanceDynamic* DynamicTransitionMaterial = UMaterialInstanceDynamic::Create(TransitionMaterial, nullptr, TEXT("TransitionMaterial"));
			DynamicTransitionMaterial->SetFlags(RF_Transient);
			
			Background->PlayTransition(NextFlipbook, DynamicTransitionMaterial, Background->IsAnimated());
			PlayAnimation(DrivingAnim, /*StartAtTime=*/0.f, /*LoopsToPlay=*/1, EUMGSequencePlayMode::Forward, /*PlaybackSpeed=*/1.f, /*RestoreState=*/true);

			OnTransitionEnd.BindUObject(this, &UVisualScene::StopTransition);
			GetWorld()->GetTimerManager().SetTimer(TransitionHandle, OnTransitionEnd, DrivingAnim->GetEndTime() + TRANSITION_THRESHOLD, false);
		}
	}
}

void UVisualScene::CancelSceneLoading()
{
	if (ActiveSceneHandle.IsValid())
	{
		ActiveSceneHandle->CancelHandle();
		ActiveSceneHandle.Reset();
	}
}

bool UVisualScene::IsSceneLoading() const
{
	if (ActiveSceneHandle.IsValid())
	{
		return ActiveSceneHandle->IsLoadingInProgress();
	}

	return false;
}

bool UVisualScene::IsSceneLoaded() const
{
	if (ActiveSceneHandle.IsValid())
	{
		return ActiveSceneHandle->HasLoadCompleted();
	}
	
	return false;
}

bool UVisualScene::CanAdvanceScene() const
{
	return Node.IsValidIndex(SceneIndex + 1);
}

bool UVisualScene::CanRetractScene() const
{
	return Node.IsValidIndex(SceneIndex - 1) || !ExhaustedScenes.IsEmpty();
}

bool UVisualScene::IsSceneExhausted(const FScenario* Scene) const
{
	return ExhaustedScenes.Contains(Scene);
}

bool UVisualScene::IsScenarioExhausted(const FScenario& Scenario) const
{
	return IsSceneExhausted(&Scenario);
}

const FScenario* UVisualScene::GetCurrentScene() const
{
	return Node[SceneIndex];
}

const FScenario& UVisualScene::GetCurrentScenario() const
{
	return *GetCurrentScene();
}

bool UVisualScene::IsWithChoice() const
{
	return GetCurrentScene()->hasChoice();
}

bool UVisualScene::IsWithTransition() const
{
	return GetCurrentScene()->hasTransition();
}

void UVisualScene::StopTransition() const
{
	Background->SetTransitionState(false);

	OnSceneTransitionEnded.Broadcast();
	OnNativeSceneTransitionEnded.Broadcast();
}

void UVisualScene::ConstructScene()
{
	ConstructScene(Node[SceneIndex]);
}

bool UVisualScene::ClearSprites()
{
	bool bRemoved = false;
	const int32 NumChildren = Canvas->GetChildrenCount();
	//Widget at index 0 is the Background, we want to keep it
	for (int32 i = 1; i < NumChildren; i++)
	{
		if (Canvas->RemoveChildAt(i))
		{
			bRemoved = true;
			i--;
		}
	}
	
	return bRemoved;
}
