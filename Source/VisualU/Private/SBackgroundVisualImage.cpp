// Fill out your copyright notice in the Description page of Project Settings.


#include "SBackgroundVisualImage.h"
#include "PaperSprite.h"
#include "PaperFlipbook.h"
#include "VisualUSettings.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture.h"
#include "TransitionMaterialProxy.h"
#include "Animation/CurveSequence.h"

SBackgroundVisualImage::SBackgroundVisualImage() 
	: SVisualImage(), 
	Transition(nullptr),
	Target(nullptr),
	bIsTransitioning(false),
	bIsTargetAnimated(false)
{
}

void SBackgroundVisualImage::SetTransition(UPaperFlipbook* TargetFlipbook, UMaterialInstanceDynamic* TransitionMaterial, bool bShouldAnimateTarget)
{
	Target = TargetFlipbook;
	Transition = TransitionMaterial;

	if (Target && bShouldAnimateTarget)
	{
		GetCurveSequence()->AddCurve(0.f, Target->GetTotalDuration());
	}

	bIsTransitioning = true;
	bIsTargetAnimated = bShouldAnimateTarget;
}

void SBackgroundVisualImage::SetTransitionState(bool IsTransitioning)
{
	bIsTransitioning = IsTransitioning;
}

UObject* SBackgroundVisualImage::GetFinalResource() const
{
	if (bIsTransitioning && Transition)
	{
		const UVisualUSettings* VisualUSettings = GetDefault<UVisualUSettings>();
		TMap<FName, UTexture*> Params;
		Params.Add(VisualUSettings->AParameterName, GetCurrentSprite()->GetBakedTexture());
		Params.Add(VisualUSettings->BParameterName, GetTargetSprite()->GetBakedTexture());
		return FTransitionMaterialProxy::GetTransitionMaterial(Transition, Params);
	}

	return GetCurrentSprite();
}

UPaperSprite* SBackgroundVisualImage::GetTargetSprite() const
{
	if (Target)
	{
		UPaperSprite* CurrentSprite = bIsTargetAnimated ? Target->GetSpriteAtTime(const_cast<SBackgroundVisualImage*>(this)->GetCurveSequence()->GetSequenceTime()) : Target->GetSpriteAtFrame(0);
		return CurrentSprite;
	}

	return nullptr;
}
