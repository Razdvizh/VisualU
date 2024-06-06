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
	FinalScene(nullptr),
	Background(nullptr),
	Canvas(nullptr)
{
}

void UVisualRenderer::DrawScene(const FScenario* Scene)
{
	check(Scene);

	ForEachSprite([&](UVisualSprite* Sprite) 
	{
		Sprite->OnSpriteDisappear.Broadcast();
		Canvas->RemoveChild(Sprite);
	});

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

			Sprite->OnSpriteAppear.Broadcast();
		}
	}
}

bool UVisualRenderer::IsTransitionInProgress() const
{
	check(Background);
	return Background->IsTransitioning();
}

bool UVisualRenderer::TryDrawTransition(const FScenario* From, const FScenario* To)
{
	check(From);
	check(To);
	const bool bIsTransitionPossible = To->Background.BackgroundArt.IsValid() &&
		From->Background.TransitionMaterial.IsValid() &&
		!Background->IsTransitioning();

	if (bIsTransitionPossible)
	{
		UPaperFlipbook* NextFlipbook = To->Background.BackgroundArt.Get();
		UMaterialInterface* TransitionMaterial = From->Background.TransitionMaterial.Get();
		UMaterialInstanceDynamic* DynamicTransitionMaterial = UMaterialInstanceDynamic::Create(TransitionMaterial, nullptr, TEXT("TransitionMaterial"));
		DynamicTransitionMaterial->SetFlags(RF_Transient | RF_DuplicateTransient | RF_TextExportTransient);

		ForEachSprite([](UVisualSprite* Sprite) 
		{
			Sprite->OnSpriteDisappear.Broadcast();
		});

		Background->PlayTransition(NextFlipbook, DynamicTransitionMaterial, Background->IsAnimated());
		PlayAnimationForward(Transition, /*PlaybackSpeed=*/1.f, /*bRestoreState=*/true);
		FinalScene = To;
	}

	return bIsTransitionPossible;
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

void UVisualRenderer::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//AnimationTabSummoner.cpp
	//MaterialParameterCollectionTrackEditor.cpp
	const UVisualUSettings* VisualUSettings = GetDefault<UVisualUSettings>();
	const float StartTime = 0.f;
	const float EndTime = VisualUSettings->TransitionDuration;

	const FName AnimationName = MakeUniqueObjectName(this, UWidgetAnimation::StaticClass());
	Transition = NewObject<UWidgetAnimation>(this, AnimationName);
	Transition->MovieScene = NewObject<UMovieScene>(Transition, AnimationName);
	Transition->MovieScene->SetDisplayRate(FFrameRate(20, 1));

	const FFrameRate TickResolution = Transition->MovieScene->GetTickResolution();
	
	const FFrameTime InFrame = StartTime * TickResolution;
	const FFrameTime OutFrame = EndTime * TickResolution;
	Transition->MovieScene->SetPlaybackRange(TRange<FFrameNumber>(InFrame.FrameNumber, OutFrame.FrameNumber + 1));

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	TSoftObjectPtr<UMaterialParameterCollection> ParameterCollection = VisualUSettings->TransitionMPC;
	
	if (ensureMsgf(!ParameterCollection.IsNull(), TEXT("Can't initialize transition animation without Parameter Collection. Please specify one in VisualU project settings.")))
	{
		UMaterialParameterCollection* Collection = ParameterCollection.LoadSynchronous();
		check(Collection);

		if (ensureMsgf(Collection->ScalarParameters.IsValidIndex(0), TEXT("No scalar parameters found to initialize transition animation. Only first scalar parameter will be used.")))
		{
			UMovieSceneMaterialParameterCollectionTrack* Track = Transition->MovieScene->AddTrack<UMovieSceneMaterialParameterCollectionTrack>();
			check(Track);

			UMovieSceneParameterSection* ParameterSection = Cast<UMovieSceneParameterSection>(Track->CreateNewSection());
			check(ParameterSection);
;
			const FName ParameterName = Collection->ScalarParameters[0].ParameterName;
			const FFrameTime EndFrame = EndTime * TickResolution;

			Track->AddSection(*ParameterSection);
			Track->MPC = Collection;
			Track->AddScalarParameterKey(ParameterName, FFrameNumber(1), 0.f);
			Track->AddScalarParameterKey(ParameterName, EndFrame.FrameNumber + 1, 1.f);
		}
	}
}

void UVisualRenderer::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);

	if (Animation == Transition)
	{
		Background->StopTransition();
		DrawScene(FinalScene);
		FinalScene = nullptr;
	}
}

void UVisualRenderer::ForEachSprite(TFunction<void(UVisualSprite* Sprite)> Action)
{
	TArray<UWidget*> Children = Canvas->GetAllChildren();
	for (UWidget*& Child : Children)
	{
		if (UVisualSprite* Sprite = Cast<UVisualSprite>(Child))
		{
			Action(Sprite);
		}
	}
}
