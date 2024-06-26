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

void UBackgroundVisualImage::StopTransition()
{
	if (VisualImageSlate.IsValid())
	{
		StaticCast<SBackgroundVisualImage*>(VisualImageSlate.Get())->StopTransition();
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
		StaticCast<SBackgroundVisualImage*>(VisualImageSlate.Get())->StartTransition(Target, Transition, bShouldAnimateTarget);
	}
}

void UBackgroundVisualImage::PlayTransition(UPaperFlipbook* Target, UMaterialInstanceDynamic* Transition, int32 TargetFrameIndex)
{
	if (VisualImageSlate.IsValid())
	{
		StaticCast<SBackgroundVisualImage*>(VisualImageSlate.Get())->StartTransition(Target, Transition, TargetFrameIndex);
	}
}

TSharedRef<SWidget> UBackgroundVisualImage::RebuildWidget()
{
	VisualImageSlate = SNew(SBackgroundVisualImage);

	return VisualImageSlate.ToSharedRef();
}
