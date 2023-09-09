// Fill out your copyright notice in the Description page of Project Settings.


#include "BackgroundVisualImage.h"
#include "SBackgroundVisualImage.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInterface.h"

void UBackgroundVisualImage::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
}

UBackgroundVisualImage::UBackgroundVisualImage(const FObjectInitializer& ObjectInitializer) : UVisualImage(ObjectInitializer)
{
}

void UBackgroundVisualImage::SetTransitionState(bool IsTransitioning)
{
	if (VisualImageSlate.IsValid())
	{
		StaticCast<SBackgroundVisualImage*>(VisualImageSlate.Get())->SetTransitionState(IsTransitioning);
	}
}

bool UBackgroundVisualImage::IsTransitioning() const
{
	if (VisualImageSlate.IsValid())
	{
		return StaticCast<SBackgroundVisualImage*>(VisualImageSlate.Get())->IsTransitioning();
	}

	return false;
}

void UBackgroundVisualImage::PlayTransition(UPaperFlipbook* Target, UMaterialInstanceDynamic* Transition, bool bShouldAnimateTarget)
{
	if (VisualImageSlate.IsValid())
	{
		StaticCast<SBackgroundVisualImage*>(VisualImageSlate.Get())->SetTransition(Target, Transition, bShouldAnimateTarget);
	}
}

TSharedRef<SWidget> UBackgroundVisualImage::RebuildWidget()
{
	VisualImageSlate = SNew(SBackgroundVisualImage);

	return VisualImageSlate.ToSharedRef();
}

#if WITH_EDITOR
const FText UBackgroundVisualImage::GetPaletteCategory()
{
	return UWidget::GetPaletteCategory();
}
#endif
