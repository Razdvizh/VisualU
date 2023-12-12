// Fill out your copyright notice in the Description page of Project Settings.


#include "SVisualImage.h"
#include "Widgets/Images/SImage.h"
#include "Animation/CurveSequence.h"
#include "PaperFlipbook.h"
#include "Rendering/DrawElements.h"
#include "PaperSprite.h"
#include "VisualDefaults.h"
#include "Engine/Texture2D.h"
#if WITH_ACCESSIBILITY
#include "Widgets/Accessibility/SlateCoreAccessibleWidgets.h"
#endif

SVisualImage::SVisualImage()
{
	SetCanTick(false);
	bCanSupportFocus = false;
	bAnimate = false;
	SpriteIndex = 0;
}

void SVisualImage::Construct(const FArguments& Args)
{
	bAnimate = Args._Animate;
	SpriteIndex = Args._SpriteIndex;
	ColorAndOpacity = Args._ColorAndOpacity;
	CustomDesiredScale = Args._CustomDesiredScale;
	Flipbook = Args._Flipbook;
	MirrorScale = Args._MirrorScale;

	UpdateSequence();
}

void SVisualImage::UpdateSequence()
{
	const UPaperFlipbook* PaperFlipbook = Flipbook.Get();

	if (PaperFlipbook)
	{
		float AnimDuration = PaperFlipbook->GetTotalDuration();
		if (AnimDuration <= 0)
		{
			AnimDuration = 0.01f;
		}
		CurveSequence = FCurveSequence();
		CurveSequence.AddCurve(0.f, AnimDuration);
		CurveSequence.Play(AsShared(), true, 0.f, false);
	}
}

#if WITH_ACCESSIBILITY
TSharedRef<FSlateAccessibleWidget> SVisualImage::CreateAccessibleWidget()
{
	return MakeShareable<FSlateAccessibleWidget>(new FSlateAccessibleImage(SharedThis(this)));
}
#endif

void SVisualImage::SetAnimate(bool IsAnimated)
{
	bAnimate = IsAnimated;
}

void SVisualImage::SetSpriteIndex(int32 Index)
{
	SpriteIndex = Index;
}

void SVisualImage::SetFlipbook(UPaperFlipbook* InFlipbook)
{
	SetFlipbook(TAttribute<const UPaperFlipbook*>(InFlipbook));

	UpdateSequence();
}

void SVisualImage::SetFlipbook(const TAttribute<const UPaperFlipbook*>& InFlipbook)
{
	SetAttribute(Flipbook, InFlipbook, EInvalidateWidgetReason::PaintAndVolatility);

	UpdateSequence();
}

void SVisualImage::SetColorAndOpacity(const TAttribute<FSlateColor>& InColorAndOpacity)
{
	SetAttribute(ColorAndOpacity, InColorAndOpacity, EInvalidateWidgetReason::Paint);
}

void SVisualImage::SetColorAndOpacity(const FLinearColor& InLinearColor)
{
	SetColorAndOpacity(TAttribute<FSlateColor>(FSlateColor(InLinearColor)));
}

void SVisualImage::SetColorAndOpacity(const FSlateColor& InSlateColor)
{
	SetColorAndOpacity(TAttribute<FSlateColor>(InSlateColor));
}

void SVisualImage::SetDesiredScale(TAttribute<TOptional<FVector2D>> InDesiredScale)
{
	SetAttribute(CustomDesiredScale, InDesiredScale, EInvalidateWidgetReason::Layout);
}

void SVisualImage::SetDesiredScale(TOptional<FVector2D> InDesiredScale)
{
	SetDesiredScale(TAttribute<TOptional<FVector2D>>(InDesiredScale));
}

void SVisualImage::SetDesiredScale(const FVector2D& InDesiredSize)
{
	SetDesiredScale(TAttribute<TOptional<FVector2D>>(TOptional<FVector2D>(InDesiredSize)));
}

void SVisualImage::SetMirrorScale(TAttribute<FVector2D> InMirrorScale)
{
	SetAttribute(MirrorScale, InMirrorScale, EInvalidateWidgetReason::Paint);
}

void SVisualImage::SetMirrorScale(const FVector2D& InMirrorScale)
{
	SetMirrorScale(TAttribute<FVector2D>(InMirrorScale));
}

void SVisualImage::SetMirrorScale(const FScale2D& InMirrorScale)
{
	const FVector2D Scale = FVector2D(InMirrorScale.GetVector());

	SetMirrorScale(TAttribute<FVector2D>(Scale));
}

UPaperSprite* SVisualImage::GetCurrentSprite() const
{
	if (const UPaperFlipbook* PaperFlipbook = Flipbook.Get())
	{
		check(PaperFlipbook->IsValidKeyFrameIndex(SpriteIndex));
		UPaperSprite* CurrentSprite = bAnimate ? PaperFlipbook->GetSpriteAtTime(CurveSequence.GetSequenceTime()) : PaperFlipbook->GetSpriteAtFrame(SpriteIndex);
		return CurrentSprite;
	}

	return nullptr;
}

FVector2D SVisualImage::ComputeDesiredSize(float) const
{
	const FSlateBrush Brush = ConvertToBrush();

	return CustomDesiredScale.Get().IsSet() ? CustomDesiredScale.Get().GetValue() * Brush.GetImageSize() : Brush.GetImageSize();
}

bool SVisualImage::ComputeVolatility() const
{
	return  SVisualImageBase::ComputeVolatility() || CurveSequence.IsPlaying() || bAnimate;
}

bool SVisualImage::IsResourceValid() const
{
	return Flipbook.Get() != nullptr;
}

UObject* SVisualImage::GetFinalResource() const
{
	return GetCurrentSprite();
}

const FVector2D SVisualImage::GetImageSize() const
{
	const FVector BoxSize = GetCurrentSprite()->GetRenderBounds().GetBox().GetSize();

	return FVector2D(BoxSize.X, BoxSize.Z);
}

const FLinearColor SVisualImage::GetFinalColorAndOpacity(const FWidgetStyle& InWidgetStyle) const
{
	return FLinearColor(InWidgetStyle.GetColorAndOpacityTint() * ColorAndOpacity.Get().GetColor(InWidgetStyle) * ConvertToBrush().GetTint(InWidgetStyle));
}

void SVisualImage::PreSlateDrawElementExtension() const
{
	//No op
}

FGeometry SVisualImage::MakeCustomGeometry(const FGeometry& AllotedGeometry) const
{
	const FVector2D ScaleToApply = MirrorScale.Get();
	if (ScaleToApply != FVector2D(1, 1) && ScaleToApply.X != 0 && ScaleToApply.Y != 0)
	{
		return AllotedGeometry.MakeChild(FSlateRenderTransform(FScale2D(ScaleToApply)));
	}

	return AllotedGeometry;
}

void SVisualImage::PostSlateDrawElementExtension() const
{
	//No op
}
