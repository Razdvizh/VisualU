// Fill out your copyright notice in the Description page of Project Settings.

#include "VisualScene.h"
#include "AssetRegistryModule.h"
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
	SceneIndex(0),
	BPScene()
{
	OnNativeSceneTransitionEnded.AddUFunction(this, TEXT("ToNextScene"));
}

TSharedRef<SWidget> UVisualScene::RebuildWidget()
{
	Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Canvas"));
	WidgetTree->RootWidget = Canvas;

	TSharedRef<SWidget> VisualSceneSlate = Super::RebuildWidget();

	Background = WidgetTree->ConstructWidget<UBackgroundVisualImage>(UBackgroundVisualImage::StaticClass(), TEXT("Background"));
	Canvas->AddChildToCanvas(Background);
	UCanvasPanelSlot* const BackgroundSlot = Cast<UCanvasPanelSlot>(Background->Slot);
	check(BackgroundSlot);
	BackgroundSlot->SetAnchors(FVisualAnchors::FullScreen);
	BackgroundSlot->SetOffsets(FVisualMargin::Zero);

	return VisualSceneSlate;
}

void UVisualScene::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (GetWidgetTreeOwningClass() != nullptr)
	{
		BPScene = GetWidgetTreeOwningClass();
	}
	const UVisualUSettings* VisualUSettings = GetDefault<UVisualUSettings>();
	const UDataTable* FirstDataTable = VisualUSettings->FirstDataTable.LoadSynchronous();
	check(FirstDataTable);
	check(FirstDataTable->GetRowStruct()->IsChildOf(FScenario::StaticStruct()));
	FirstDataTable->GetAllRows(TEXT("VisualScene.cpp(77)"), Node);
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
		UVisualSprite* const Sprite = WidgetTree->ConstructWidget<UVisualSprite>(SpriteData.SpriteClass.Get(), SpriteData.SpriteClass->GetFName());
		Sprite->AssignSpriteInfo(SpriteData.SpriteInfo);
		Canvas->AddChildToCanvas(Sprite);
		UCanvasPanelSlot* const SpriteSlot = Cast<UCanvasPanelSlot>(Sprite->Slot);
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
	ClearSprites();

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

	//\todo Check if Node resetting is necessary
	Node.Empty();
	Scene->Owner->GetAllRows(TEXT("VisualScene.cpp(260)"), Node);
	SceneIndex = Scene->Index;
	LoadAndConstruct();

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();
}

void UVisualScene::ToNode(const UDataTable* NewNode)
{
	ExhaustedScenes.Push(Node.Last());

	//\todo Check if Node resetting is necessary
	TArray<FScenario*> Rows;
	NewNode->GetAllRows(TEXT("VisualScene.cpp(274)"), Rows);
	Node = Rows;

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

void UVisualScene::PrintScenesData(const TArray<FAssetData>& InScenesData) const
{
	for (const auto& Asset : InScenesData)
	{
		const UDataTable* DataTable = Cast<UDataTable>(Asset.GetAsset());
		TArray<FScenario*> Rows;

		DataTable->GetAllRows(TEXT("VisualScene.cpp(388)"), Rows);

		UE_LOG(LogVisualU, Warning, TEXT("%s"), *Asset.AssetName.ToString());

		int cnt = 0;
		for (const auto Row : Rows)
		{
			cnt++;
			UE_LOG(LogVisualU, Warning, TEXT("\tRow %d"), cnt);
			Row->PrintLog();
			UE_LOG(LogVisualU, Warning, TEXT("================================================="));
		}
	}
}

void UVisualScene::GetScenesData(TArray<FAssetData>& OutData) const
{
	FAssetRegistryModule* AssetRegistryModule = &FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	FARFilter Filter;

	const FName Name = UDataTable::StaticClass()->GetFName();
	const FName Key = TEXT("RowStructure");
	const FString Value = TEXT("Scenario");

	Filter.ClassNames.Add(Name);
	Filter.bIncludeOnlyOnDiskAssets = true;
	Filter.TagsAndValues.AddUnique(Key, Value);

	if (IsInGameThread())
	{
		AssetRegistryModule->Get().GetAssets(Filter, OutData);
	}
}

void UVisualScene::StopTransition() const
{
	Background->SetTransitionState(false);

	OnNativeSceneTransitionEnded.Broadcast();
}

void UVisualScene::ConstructScene()
{
	ConstructScene(Node[SceneIndex]);
}

bool UVisualScene::ClearSprites()
{
	bool bRemoved = false;
	const int32 NumSprites = Canvas->GetChildrenCount();
	//Widget at index 0 is the Background, we want to keep it
	for (int32 i = 1; i < NumSprites; i++)
	{
		if (Canvas->RemoveChildAt(i))
		{
			bRemoved = true;
		}
	}
	
	return bRemoved;
}
