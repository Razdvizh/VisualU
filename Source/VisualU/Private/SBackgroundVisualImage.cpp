// Copyright (c) 2024 Evgeny Shustov


#include "SBackgroundVisualImage.h"
#include "PaperSprite.h"
#include "PaperFlipbook.h"
#include "VisualUSettings.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ME_TransitionParameter2D.h"
#include "Engine/Texture.h"
#include "Animation/CurveSequence.h"

SLATE_IMPLEMENT_WIDGET(SBackgroundVisualImage)
void SBackgroundVisualImage::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, TEXT("Target"), Target, EInvalidateWidgetReason::LayoutAndVolatility);
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, TEXT("Transition"), Transition, EInvalidateWidgetReason::Paint);
}

SBackgroundVisualImage::SBackgroundVisualImage() 
	: SVisualImage(), 
	Transition(*this),
	Target(*this),
	bIsTransitioning(false),
	bIsTargetAnimated(false),
	TargetFrameIndex(0)
{
}

void SBackgroundVisualImage::Construct(const FArguments& Args)
{
	bIsTransitioning = Args._IsTransitioning;
	bIsTargetAnimated = Args._IsTargetAnimated;
	TargetFrameIndex = Args._TargetFrameIndex;
	Transition.Assign(*this, Args._Transition);
	Target.Assign(*this, Args._Target);
}

void SBackgroundVisualImage::StartTransition(UPaperFlipbook* TargetFlipbook, UMaterialInstanceDynamic* TransitionMaterial, bool bShouldAnimateTarget)
{
	Target.Set(*this, TargetFlipbook);
	Transition.Set(*this, TransitionMaterial);

	if (Target.Get() && bShouldAnimateTarget)
	{
		GetCurveSequence()->AddCurve(0.f, Target.Get()->GetTotalDuration());
		GetCurveSequence()->Play(AsShared(), true, 0.f, false);
	}

	bIsTransitioning = true;
	bIsTargetAnimated = bShouldAnimateTarget;
	TargetFrameIndex = 0;
}

void SBackgroundVisualImage::StartTransition(UPaperFlipbook* TargetFlipbook, UMaterialInstanceDynamic* TransitionMaterial, int32 FrameIndex)
{
	Target.Set(*this, TargetFlipbook);
	Transition.Set(*this, TransitionMaterial);

	check(TargetFrameIndex >= 0);
	TargetFrameIndex = FrameIndex;

	bIsTransitioning = true;
	bIsTargetAnimated = false;
}

void SBackgroundVisualImage::StopTransition()
{
	SetFlipbook(Target.Get());
	bIsTransitioning = false;
	UpdateSequence();
}

void SBackgroundVisualImage::AddReferencedObjects(FReferenceCollector& Collector)
{
	Super::AddReferencedObjects(Collector);
	
	UMaterialInstanceDynamic* TransitionPtr = Transition.Get();
	Collector.AddReferencedObject(TransitionPtr);

	UPaperFlipbook* TargetPtr = Target.Get();
	Collector.AddReferencedObject(TargetPtr);
}

FString SBackgroundVisualImage::GetReferencerName() const
{
	return TEXT("SBackgroundVisualImage");
}

UObject* SBackgroundVisualImage::GetFinalResource() const
{
	if (bIsTransitioning && Transition.Get())
	{
		const UVisualUSettings* VisualUSettings = GetDefault<UVisualUSettings>();
		TMap<FName, UTexture*> Params;
		Params.Add(VisualUSettings->AParameterName, GetCurrentSprite()->GetBakedTexture());
		Params.Add(VisualUSettings->BParameterName, GetTargetSprite()->GetBakedTexture());
		return UME_TransitionParameter2D::GetTransitionMaterial(Transition.Get(), Params);
	}

	return GetCurrentSprite();
}

UPaperSprite* SBackgroundVisualImage::GetTargetSprite() const
{
	if (UPaperFlipbook* TargetFlipbook = Target.Get())
	{
		UPaperSprite* CurrentSprite = bIsTargetAnimated ? TargetFlipbook->GetSpriteAtTime(const_cast<SBackgroundVisualImage*>(this)->GetCurveSequence()->GetSequenceTime()) : TargetFlipbook->GetSpriteAtFrame(TargetFrameIndex);
		return CurrentSprite;
	}

	return nullptr;
}
