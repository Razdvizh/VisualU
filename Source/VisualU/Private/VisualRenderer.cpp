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

UVisualRenderer::UVisualRenderer(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{

}

TSharedRef<SWidget> UVisualRenderer::RebuildWidget()
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
