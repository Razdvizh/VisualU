// Copyright (c) 2024 Evgeny Shustov


#include "VisualRenderer.h"
#include "Blueprint/WidgetTree.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialParameterCollection.h"
#include "Tracks/MovieSceneMaterialParameterCollectionTrack.h"
#include "Sections/MovieSceneParameterSection.h"
#include "MovieSceneSection.h"
#include "MovieScene.h"
#include "Animation/WidgetAnimation.h"
#include "Animation/UMGSequencePlayer.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Containers/Ticker.h"
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
	Canvas(nullptr),
	DrawHandle()
{
}

void UVisualRenderer::DrawScene(const FScenario* Scene)
{
	check(Scene);
	check(WidgetTree);

	ForEachSprite([this](UVisualSprite* Sprite) 
	{
		Canvas->RemoveChild(Sprite);
		WidgetTree->RemoveWidget(Sprite);

		Sprite->OnSpriteDisappear.Broadcast();
	});

	if (!Scene->Info.Background.BackgroundArtInfo.Expression.IsNull())
	{
		Background->AssignVisualImageInfo(Scene->Info.Background.BackgroundArtInfo);
	}
	
	using FDrawRequest = TDelegate<void()>;

	TArray<FDrawRequest> SpriteDrawRequests;
	SpriteDrawRequests.Reserve(Scene->Info.SpritesParams.Num());

	for (const FSprite& SpriteData : Scene->Info.SpritesParams)
	{
		if (UClass* const SpriteClass = SpriteData.SpriteClass.Get())
		{
			const FName SpriteName = SpriteClass->GetFName();
			UVisualSprite* Sprite = WidgetTree->ConstructWidget<UVisualSprite>(SpriteClass, SpriteName);
			ESlateVisibility FinalVisibility = Sprite->GetVisibility();
			Sprite->SetVisibility(ESlateVisibility::Hidden);
			Sprite->AssignSpriteInfo(SpriteData.SpriteInfo);

			UCanvasPanelSlot* SpriteSlot = Canvas->AddChildToCanvas(Sprite);
			check(SpriteSlot);

			SpriteSlot->SetZOrder(SpriteData.ZOrder);
			SpriteSlot->SetAnchors(SpriteData.Anchors);
			SpriteSlot->SetAutoSize(true);
			
			SpriteDrawRequests.Add(FDrawRequest::CreateWeakLambda(this, [this, SpriteName, SpriteData, FinalVisibility]
			{
				UVisualSprite* Sprite = WidgetTree->FindWidget<UVisualSprite>(SpriteName);

				if (IsValid(Sprite) && IsValid(Sprite->Slot))
				{
					const FVector2D Size = Sprite->GetDesiredSize();

					const bool bZeroAnchors = (SpriteData.Anchors.Minimum.IsZero() || SpriteData.Anchors.Maximum.IsZero());
					const float XAnchor =
						bZeroAnchors
						? 0.f
						: FMath::IsNearlyEqual(SpriteData.Anchors.Minimum.X, SpriteData.Anchors.Maximum.X)
						? SpriteData.Anchors.Minimum.X
						: FMath::Abs(SpriteData.Anchors.Maximum.X - SpriteData.Anchors.Minimum.X);

					const float YAnchor =
						bZeroAnchors
						? 0.f
						: FMath::IsNearlyEqual(SpriteData.Anchors.Minimum.Y, SpriteData.Anchors.Maximum.Y)
						? SpriteData.Anchors.Minimum.Y
						: FMath::Abs(SpriteData.Anchors.Maximum.Y - SpriteData.Anchors.Minimum.Y);

					const FVector2D AnchorModifier = FVector2D(XAnchor, YAnchor);
					const FVector2D SpritePosition = (Size * AnchorModifier * -1) + SpriteData.Position;

					UCanvasPanelSlot* SpriteSlot = Cast<UCanvasPanelSlot>(Sprite->Slot);
					check(SpriteSlot);

					SpriteSlot->SetPosition(SpritePosition);
					Sprite->SetVisibility(FinalVisibility);

					Sprite->OnSpriteAppear.Broadcast();
				}
			}));
		}
	}

	/*give time for Slate to fill in the cache so that it is possible to calculate position*/
	DrawHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateWeakLambda(this, [this, SpriteDrawRequests](float)
	{
		static int32 Frames = 0;

		Frames++;

		/*Skip first frame*/
		if (Frames == 1)
		{
			return true;
		}
		/*Force layout prepass on second frame*/
		else if (Frames == 2)
		{
			ForEachSprite([](UVisualSprite* Sprite)
			{
				Sprite->ForceLayoutPrepass();
			});

			return true;
		}
		/*Skip third frame*/
		else if (Frames == 3)
		{
			return true;
		}
		
		/*Finally render the sprites on fifth frame*/
		for (const FDrawRequest& DrawRequest : SpriteDrawRequests)
		{
			DrawRequest.ExecuteIfBound();
		};

		Frames = 0;

		return false;
	}));
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
	const FVisualImageInfo& ToBackgroundArtInfo = To->Info.Background.BackgroundArtInfo;
	const TSoftObjectPtr<UMaterialInterface>& SoftTransitionMaterial = From->Info.Background.TransitionMaterial;
	const bool bIsTransitionPossible = ToBackgroundArtInfo.Expression.IsValid()
											 && SoftTransitionMaterial.IsValid()
											 &&	 !Background->IsTransitioning();

	if (bIsTransitionPossible)
	{
		UPaperFlipbook* NextFlipbook = ToBackgroundArtInfo.Expression.Get();
		UMaterialInterface* TransitionMaterial = SoftTransitionMaterial.Get();
		UMaterialInstanceDynamic* DynamicTransitionMaterial = UMaterialInstanceDynamic::Create(TransitionMaterial, nullptr, TEXT("TransitionMaterial"));
		DynamicTransitionMaterial->SetFlags(RF_Transient | RF_DuplicateTransient | RF_TextExportTransient);

		ForEachSprite([this](UVisualSprite* Sprite) 
		{
			Canvas->RemoveChild(Sprite);
			WidgetTree->RemoveWidget(Sprite);

			Sprite->OnSpriteDisappear.Broadcast();
		});
		
		FinalScene = To;
		if (ToBackgroundArtInfo.bAnimate)
		{
			Background->PlayTransition(NextFlipbook, DynamicTransitionMaterial, true);
		}
		else
		{
			Background->PlayTransition(NextFlipbook, DynamicTransitionMaterial, ToBackgroundArtInfo.FrameIndex);
		}
		PlayAnimationForward(Transition, /*PlaybackSpeed=*/1.f, /*bRestoreState=*/true);
	}

	return bIsTransitionPossible;
}

void UVisualRenderer::ForceStopTransition()
{
	if (IsTransitionInProgress())
	{
		UUMGSequencePlayer* Player = GetSequencePlayer(Transition);
		check(Player);

		Player->Stop();

		UpdateCanTick();
	}
}

TSharedRef<SWidget> UVisualRenderer::RebuildWidget()
{
	check(WidgetTree);
	Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Canvas"));
	WidgetTree->RootWidget = Canvas;

	Background = WidgetTree->ConstructWidget<UBackgroundVisualImage>(UBackgroundVisualImage::StaticClass(), TEXT("Background"));

	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateWeakLambda(this, [this](float)
	{
		UCanvasPanelSlot* BackgroundSlot = Canvas->AddChildToCanvas(Background);
		check(BackgroundSlot);
		BackgroundSlot->SetAnchors(FVisualAnchors::FullScreen);
		BackgroundSlot->SetOffsets(FVisualMargin::Zero);
		BackgroundSlot->SetZOrder(INT_MIN);

		return false;
	}));

	return Super::RebuildWidget();
}

void UVisualRenderer::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	const UVisualUSettings* VisualUSettings = GetDefault<UVisualUSettings>();
	constexpr float StartTime = 0.f;
	const float EndTime = VisualUSettings->TransitionDuration;

	const FName AnimationName = MakeUniqueObjectName(this, UWidgetAnimation::StaticClass());
	Transition = NewObject<UWidgetAnimation>(this, AnimationName);
	Transition->MovieScene = NewObject<UMovieScene>(Transition, AnimationName);
	Transition->MovieScene->SetDisplayRate(FFrameRate(20, 1));
	
	const FFrameRate TickResolution = Transition->MovieScene->GetTickResolution();
	
	const FFrameTime InFrame = StartTime * TickResolution;
	const FFrameTime OutFrame = EndTime * TickResolution;
	Transition->MovieScene->SetPlaybackRange(TRange<FFrameNumber>(InFrame.FrameNumber, OutFrame.FrameNumber + 1));

	TSoftObjectPtr<UMaterialParameterCollection> ParameterCollection = VisualUSettings->TransitionMPC;
	
	if (ensureMsgf(!ParameterCollection.IsNull(), TEXT("Can't initialize transition animation without parameter collection. Please specify one in VisualU project settings.")))
	{
		UMaterialParameterCollection* Collection = ParameterCollection.LoadSynchronous();
		check(Collection);

		if (ensureMsgf(!Collection->ScalarParameters.IsEmpty(), TEXT("No scalar parameters found to initialize transition animation. Please specify one scalar parameter in parameter collection.")))
		{
			UMovieSceneMaterialParameterCollectionTrack* Track = Transition->MovieScene->AddTrack<UMovieSceneMaterialParameterCollectionTrack>();
			check(Track);

			UMovieSceneParameterSection* ParameterSection = Cast<UMovieSceneParameterSection>(Track->CreateNewSection());
			check(ParameterSection);

			const FName& ParameterName = Collection->ScalarParameters[0].ParameterName;
			const FFrameTime EndFrame = EndTime * TickResolution;

			Track->AddSection(*ParameterSection);
			Track->MPC = Collection;
			Track->AddScalarParameterKey(ParameterName, FFrameNumber(1), /*Value=*/0.f);
			Track->AddScalarParameterKey(ParameterName, EndFrame.FrameNumber + 1, /*Value=*/1.f);
		}
	}
}

void UVisualRenderer::NativeDestruct()
{
	FTSTicker::GetCoreTicker().RemoveTicker(DrawHandle);

	Super::NativeDestruct();
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
