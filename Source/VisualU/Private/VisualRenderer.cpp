// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualRenderer.h"
#include "Blueprint/WidgetTree.h"
#include "Materials/MaterialInterface.h"
#include "TransitionMaterialProxy.h"
#include "Materials/MaterialParameterCollection.h"
#include "Tracks/MovieSceneMaterialParameterCollectionTrack.h"
#include "Sections/MovieSceneParameterSection.h"
#include "MovieSceneSection.h"
#include "MovieScene.h"
#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Sound/SoundCue.h"
#include "PaperFlipbook.h"
#include "VisualSprite.h"
#include "VisualImage.h"
#include "VisualDefaults.h"
#include "VisualUSettings.h"
#include "BackgroundVisualImage.h"

UVisualRenderer::UVisualRenderer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	Transition(nullptr),
	Background(nullptr),
	Canvas(nullptr)
{
	const UVisualUSettings* VisualUSettings = GetDefault<UVisualUSettings>();
	ParameterCollection = VisualUSettings->MPC;
}

void UVisualRenderer::DrawScene(const FScenario* Scene)
{
	check(Scene);

	ClearSprites();

	if (UPaperFlipbook* const BackgroundArt = Scene->Background.BackgroundArt.Get())
	{
		Background->SetFlipbook(BackgroundArt);
	}

	if (USoundBase* const Music = Scene->Music.Get())
	{
		PlaySound(Music);
	}

	for (const auto& SpriteData : Scene->SpritesParams)
	{
		if (UClass* const SpriteClass = SpriteData.SpriteClass.Get())
		{
			UVisualSprite* Sprite = WidgetTree->ConstructWidget<UVisualSprite>(SpriteClass, SpriteClass->GetFName());
			Sprite->AssignSpriteInfo(SpriteData.SpriteInfo);

			UCanvasPanelSlot* SpriteSlot = Canvas->AddChildToCanvas(Sprite);
			SpriteSlot->SetZOrder(SpriteData.ZOrder);
			SpriteSlot->SetAnchors(SpriteData.Anchors);
			SpriteSlot->SetAutoSize(true);
			SpriteSlot->SetPosition(SpriteData.Position);
		}
	}
}

void UVisualRenderer::PlayTransition(const FScenario* From, const FScenario* To)
{
	check(From);
	check(To);
	const TSoftObjectPtr<UMaterialInterface> SoftTransitionMaterial = From->Background.TransitionMaterial;
	if (!SoftTransitionMaterial.IsNull())
	{
		const bool bIsTransitionPossible = ensureMsgf(!To->Background.BackgroundArt.IsNull(), TEXT("You are trying to transition to an empty background"));
		if (bIsTransitionPossible)
		{
			UPaperFlipbook* NextFlipbook = To->Background.BackgroundArt.LoadSynchronous();
			UMaterialInterface* TransitionMaterial = SoftTransitionMaterial.LoadSynchronous();
			UMaterialInstanceDynamic* DynamicTransitionMaterial = UMaterialInstanceDynamic::Create(TransitionMaterial, nullptr, TEXT("TransitionMaterial"));
			DynamicTransitionMaterial->SetFlags(RF_Transient | RF_DuplicateTransient | RF_TextExportTransient);

			Background->PlayTransition(NextFlipbook, DynamicTransitionMaterial, Background->IsAnimated());
			PlayAnimationForward(Transition, 1.f, true);
		}
	}
}

TSharedRef<SWidget> UVisualRenderer::RebuildWidget()
{
	Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Canvas"));
	WidgetTree->RootWidget = Canvas;

	TSharedRef<SWidget> VisualSceneSlate = Super::RebuildWidget();

	Background = WidgetTree->ConstructWidget<UBackgroundVisualImage>(UBackgroundVisualImage::StaticClass(), TEXT("Background"));
	UCanvasPanelSlot* BackgroundSlot = Canvas->AddChildToCanvas(Background);
	check(BackgroundSlot);
	BackgroundSlot->SetAnchors(FVisualAnchors::FullScreen);
	BackgroundSlot->SetOffsets(FVisualMargin::Zero);
	BackgroundSlot->SetZOrder(INT_MIN);

	return VisualSceneSlate;
}

void UVisualRenderer::NativeConstruct()
{
	Super::NativeConstruct();

	//AnimationTabSummoner.cpp
	//MaterialParameterCollectionTrackEditor.cpp
	const float StartTime = 0.f;
	const float EndTime = 1.f;

	const FName AnimationName = MakeUniqueObjectName(this, UWidgetAnimation::StaticClass());
	Transition = NewObject<UWidgetAnimation>(this, AnimationName);
	Transition->MovieScene = NewObject<UMovieScene>(Transition, AnimationName);
	Transition->MovieScene->SetDisplayRate(FFrameRate(20, 1));
	const FFrameTime InFrame = StartTime * Transition->MovieScene->GetTickResolution();
	const FFrameTime OutFrame = EndTime * Transition->MovieScene->GetTickResolution();
	Transition->MovieScene->SetPlaybackRange(TRange<FFrameNumber>(InFrame.FrameNumber, OutFrame.FrameNumber + 1));

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (!ParameterCollection.IsNull())
	{
		UMaterialParameterCollection* Collection = ParameterCollection.LoadSynchronous();

		Transition->MovieScene->Modify();

		UMovieSceneMaterialParameterCollectionTrack* Track = Transition->MovieScene->AddTrack<UMovieSceneMaterialParameterCollectionTrack>();
		check(Track);

		UMovieSceneParameterSection* ParameterSection = Cast<UMovieSceneParameterSection>(Track->CreateNewSection());
		check(ParameterSection);

		const int32 TicksPerFrame = Transition->MovieScene->GetTickResolution().AsDecimal() / Transition->MovieScene->GetDisplayRate().AsDecimal();
		Track->AddSection(*ParameterSection);
		Track->MPC = Collection;
		Track->AddScalarParameterKey(TEXT("Blending"), FFrameNumber(1), 0.f);
		Track->AddScalarParameterKey(TEXT("Blending"), FFrameNumber(21 * TicksPerFrame), 1.f);
	}
}

bool UVisualRenderer::ClearSprites()
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
