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
#include "VisualUSettings.h"
#include "Sound/SoundCue.h"
#include "PaperFlipbook.h"

UVisualScene* UVisualScene::Get()
{
	for (TObjectIterator<UVisualScene> Itr; Itr; ++Itr)
	{
		if (Itr->IsInViewport())
		{
			return *Itr;
			break;
		}
	}
	
	return nullptr;
}

UVisualScene::UVisualScene(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), SceneIndex(0)
{

}

TSharedRef<SWidget> UVisualScene::RebuildWidget()
{
	Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Canvas"));
	WidgetTree->RootWidget = Canvas;

	TSharedRef<SWidget> VisualSceneSlate = Super::RebuildWidget();

	Background = WidgetTree->ConstructWidget<UVisualImage>(UVisualImage::StaticClass(), TEXT("Background"));
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

	BPScene = GetWidgetTreeOwningClass();
	const UVisualUSettings* VisualUSettings = GetDefault<UVisualUSettings>();
	const UDataTable* FirstDataTable = VisualUSettings->FirstDataTable.LoadSynchronous();
	check(FirstDataTable);
	check(FirstDataTable->GetRowStruct()->IsChildOf(FScenario::StaticStruct()));
	FirstDataTable->GetAllRows(TEXT("VisualScene.cpp(65)"), Branch);
	checkf(Branch.IsValidIndex(0), TEXT("First scene is empty!"));
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

	Background->SetFlipbook(Scene->BackgroundArt.Get());
	
	PlaySound(Scene->SceneStartAudioEffect.Get());
	PlaySound(Scene->Music.Get());
	for (const auto& SpriteData : Scene->SpritesParams)
	{
		UVisualSprite* const Sprite = WidgetTree->ConstructWidget<UVisualSprite>(SpriteData.SpriteClass, SpriteData.SpriteClass->GetFName());
		Sprite->AssignExpressions(SpriteData.Expressions);
		Canvas->AddChildToCanvas(Sprite);
		UCanvasPanelSlot* const SpriteSlot = Cast<UCanvasPanelSlot>(Sprite->Slot);
		SpriteSlot->SetZOrder(SpriteData.ZOrder);
		SpriteSlot->SetAnchors(FVisualAnchors::BottomLeft);
		SpriteSlot->SetAutoSize(true);
		SpriteSlot->SetPosition(SpriteData.Position);

		OnSceneConstructed.Broadcast();
		OnNativeSceneConstructed.Broadcast();
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
	const FScenario* Scene = Branch[SceneIndex];
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
	if (Branch[0]->Owner == Scene->Owner)
	{
		isFound = true;
	}
	else
	{
		/*Traverse the stack until the requested branch is found*/
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

bool UVisualScene::ToScenario(const FScenario& Scenario)
{
	return ToScene(&Scenario);
}

void UVisualScene::SetCurrentScene(const FScenario* Scene)
{
	OnSceneEnd.Broadcast();
	OnNativeSceneEnd.Broadcast();

	Branch.Empty();
	Scene->Owner->GetAllRows(TEXT("VisualScene.cpp(220)"), Branch);
	SceneIndex = Scene->Index;
	LoadAndConstruct();

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();
}

void UVisualScene::NativeToBranch(const TArray<FScenario*>& Rows)
{
	ExhaustedScenes.Push(Branch.Last());
	Branch = Rows;

	OnSceneEnd.Broadcast();
	OnNativeSceneEnd.Broadcast();

	SceneIndex = 0;
	LoadAndConstruct();

	OnSceneStart.Broadcast();
	OnNativeSceneStart.Broadcast();
}

void UVisualScene::ToBranch(UPARAM(ref) TArray<FScenario>& Rows)
{
	TArray<FScenario*> RowsPtr;
	for (auto& Row : Rows)
	{
		RowsPtr.Add(&Row);
	}

	NativeToBranch(MoveTemp(RowsPtr));
}

/*
void UVisualScene::AutoProceedScenes(UVisualSceneComponent* Component) {}
void UVisualScene::FastForward(UVisualSceneComponent* Component, float speedMultiplier) {}
void UVisualScene::Rewind(UVisualSceneComponent* Component, float speedMultiplier) {}
*/

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
	return Branch.IsValidIndex(SceneIndex + 1);
}

bool UVisualScene::CanRetractScene() const
{
	return Branch.IsValidIndex(SceneIndex - 1);
}

bool UVisualScene::IsSceneExhausted(const FScenario* Scene) const
{
	return ExhaustedScenes.Contains(Scene);
}

bool UVisualScene::IsScenarioExhausted(const FScenario& Scenario) const
{
	return IsSceneExhausted(&Scenario);
}

const FText UVisualScene::GetLine() const
{
	return Branch[SceneIndex]->Line;
}

const FText UVisualScene::GetAuthor() const
{
	return Branch[SceneIndex]->Author;
}

const FScenario* UVisualScene::GetCurrentScene() const
{
	return Branch[SceneIndex];
}

const FScenario& UVisualScene::GetCurrentScenario() const
{
	return *GetCurrentScene();
}

bool UVisualScene::IsWithChoice() const
{
	return GetCurrentScene()->hasChoice();
}

/*
* Outputs a friendly representation of scene's data to the log
* @param InScenesData asset data of Scenario Data Tables
* @see GetScenesData
*/
void UVisualScene::PrintScenesData(const TArray<FAssetData>& InScenesData) const
{
	for (const auto& Asset : InScenesData)
	{
		const UDataTable* DataTable = Cast<UDataTable>(Asset.GetAsset());
		TArray<FScenario*> Rows;

		DataTable->GetAllRows(TEXT("VisualScene.cpp(346)"), Rows);

		UE_LOG(LogTemp, Warning, TEXT("%s"), *Asset.AssetName.ToString());

		int cnt = 0;
		for (const auto Row : Rows)
		{
			cnt++;
			UE_LOG(LogTemp, Warning, TEXT("\tRow %d"), cnt);
			Row->ToString();
		}
	}
}

/*
* Gathers asset data of all Scenario Data Tables using Asset Registry
* @param OutData gathered asset data
*/
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

void UVisualScene::ConstructScene()
{
	ConstructScene(Branch[SceneIndex]);
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
