// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualRenderer.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "VisualSprite.h"
#include "VisualImage.h"
#include "VisualDefaults.h"
#include "BackgroundVisualImage.h"
#include "Sound/SoundCue.h"
#include "PaperFlipbook.h"

UVisualRenderer::UVisualRenderer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	Transition(nullptr),
	Background(nullptr),
	Canvas(nullptr)
{

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
