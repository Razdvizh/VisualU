// Fill out your copyright notice in the Description page of Project Settings.


#include "SVisualImage.h"
#include "Widgets\Images\SImage.h"
#include "Animation\CurveSequence.h"
#include "PaperFlipbook.h"
#include "Rendering\DrawElements.h"
#include "PaperSprite.h"
#include "VisualDefaults.h"
#if WITH_ACCESSIBILITY
#include "Widgets\Accessibility\SlateCoreAccessibleWidgets.h"
#endif

SLATE_IMPLEMENT_WIDGET(SVisualImage)
void SVisualImage::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, TEXT("Flipbook"), Flipbook, EInvalidateWidgetReason::LayoutAndVolatility);
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, TEXT("Color and Opacity"), ColorAndOpacity, EInvalidateWidgetReason::Paint);
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, TEXT("Custom Desired Scale"), CustomDesiredScale, EInvalidateWidgetReason::Layout);
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, TEXT("Mirror Scale"), MirrorScale, EInvalidateWidgetReason::Paint);
}

SVisualImage::SVisualImage() : Flipbook(*this),
	ColorAndOpacity(*this, FLinearColor(ForceInit)), 
	CustomDesiredScale(*this, FVector2D(ForceInitToZero)), 
	MirrorScale(*this, FVector2D(ForceInitToZero))
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
	ColorAndOpacity.Assign(*this, Args._ColorAndOpacity);
	CustomDesiredScale.Assign(*this, Args._CustomDesiredScale);
	Flipbook.Assign(*this, Args._Flipbook);
	MirrorScale.Assign(*this, Args._MirrorScale);

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

void SVisualImage::SetSpriteIndex(int Index)
{
	SpriteIndex = Index;
}

void SVisualImage::SetFlipbook(UPaperFlipbook* InFlipbook)
{
	Flipbook.Set(*this, InFlipbook);

	UpdateSequence();
}

void SVisualImage::SetFlipbook(TAttribute<const UPaperFlipbook*> InFlipbook)
{
	Flipbook.Assign(*this, MoveTemp(InFlipbook));

	UpdateSequence();
}

void SVisualImage::SetColorAndOpacity(TAttribute<FSlateColor> InColorAndOpacity)
{
	ColorAndOpacity.Assign(*this, MoveTemp(InColorAndOpacity));
}

void SVisualImage::SetColorAndOpacity(const FLinearColor& InLinearColor)
{
	ColorAndOpacity.Set(*this, FSlateColor(InLinearColor));
}

void SVisualImage::SetColorAndOpacity(const FSlateColor& InSlateColor)
{
	ColorAndOpacity.Set(*this, InSlateColor);
}

void SVisualImage::SetDesiredScale(TAttribute<TOptional<FVector2D>> InDesiredScale)
{
	CustomDesiredScale.Assign(*this, MoveTemp(InDesiredScale));
}

void SVisualImage::SetDesiredScale(TOptional<FVector2D> InDesiredScale)
{
	CustomDesiredScale.Set(*this, InDesiredScale);
}

void SVisualImage::SetDesiredScale(const FVector2D& InDesiredSize)
{
	CustomDesiredScale.Set(*this, TOptional<FVector2D>(InDesiredSize));
}

void SVisualImage::SetMirrorScale(TAttribute<FVector2D> InMirrorScale)
{
	MirrorScale.Assign(*this, MoveTemp(InMirrorScale));
}

void SVisualImage::SetMirrorScale(const FVector2D& InMirrorScale)
{
	MirrorScale.Set(*this, InMirrorScale);
}

void SVisualImage::SetMirrorScale(const FScale2D& InMirrorScale)
{
	const FVector2D Scale = FVector2D(InMirrorScale.GetVector());

	MirrorScale.Set(*this, Scale);
}

UPaperSprite* SVisualImage::GetCurrentSprite() const
{
	const UPaperFlipbook* PaperFlipbook = Flipbook.Get();
	if (PaperFlipbook)
	{
		check(PaperFlipbook->IsValidKeyFrameIndex(SpriteIndex));
		UPaperSprite* CurrentSprite = bAnimate ? PaperFlipbook->GetSpriteAtTime(CurveSequence.GetSequenceTime()) : PaperFlipbook->GetSpriteAtFrame(SpriteIndex);
		return CurrentSprite;
	}

	return nullptr;
}

FVector2D SVisualImage::ComputeDesiredSize(float) const
{
	const FSlateBrush Brush = ConvertFlipbookToBrush();
	
	return CustomDesiredScale.Get().IsSet() ? CustomDesiredScale.Get().GetValue() * Brush.GetImageSize() : Brush.GetImageSize();
}

bool SVisualImage::ComputeVolatility() const
{
	return Super::ComputeVolatility() || CurveSequence.IsPlaying() || bAnimate;
}

int32 SVisualImage::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const FSlateBrush Brush = ConvertFlipbookToBrush();

	const FLinearColor FinalColorAndOpacity = FLinearColor(InWidgetStyle.GetColorAndOpacityTint() * ColorAndOpacity.Get().GetColor(InWidgetStyle) * Brush.GetTint(InWidgetStyle));
	const FVector2D ScaleToApply = MirrorScale.Get();

	if (Brush.GetResourceObject() != nullptr)
	{
		if (ScaleToApply != FVector2D(1, 1) && ScaleToApply.X != 0 && ScaleToApply.Y != 0)
		{
			const FGeometry MirroredGeometry = AllottedGeometry.MakeChild(FSlateRenderTransform(FScale2D(ScaleToApply)));
			FSlateDrawElement::MakeBox(OutDrawElements, LayerId, MirroredGeometry.ToPaintGeometry(), &Brush, ESlateDrawEffect::None, FinalColorAndOpacity);
		}
		else
		{
			FSlateDrawElement::MakeBox(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), &Brush, ESlateDrawEffect::None, FinalColorAndOpacity);
		}
	}

	return LayerId;
}

FSlateBrush SVisualImage::ConvertFlipbookToBrush() const
{
	FSlateBrush Brush = FSlateBrush();

	if (Flipbook.Get())
	{
		UPaperSprite* CurrentSprite = GetCurrentSprite();
		
		checkSlow(CurrentSprite);

		const FVector3d BoxSize = CurrentSprite->GetRenderBounds().GetBox().GetSize();

		const FVector2D ImageSize = FVector2D(BoxSize.X, BoxSize.Z);

		Brush.SetResourceObject(CurrentSprite);
		Brush.ImageSize = ImageSize;
		Brush.ImageType = ESlateBrushImageType::FullColor;
		Brush.DrawAs = ESlateBrushDrawType::Image;
		Brush.Tiling = ESlateBrushTileType::NoTile;
		Brush.TintColor = FSlateColor(FLinearColor::White);
	}
	return Brush;
}
