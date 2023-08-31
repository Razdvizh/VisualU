// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/CurveSequence.h"
#include "SVisualImageBase.h"

class UPaperFlipbook;
class UPaperSprite;

/// <summary>
/// Slate widget that displays sprite flipbooks.
/// </summary>
/// <seealso cref="UVisualImage"/>
class VISUALU_API SVisualImage : public SVisualImageBase<SVisualImage>
{

	SLATE_DECLARE_WIDGET(SVisualImage, SVisualImageBase<SVisualImage>)

public:
	SLATE_BEGIN_ARGS(SVisualImage) : _Animate(false), _SpriteIndex(0) {}

		SLATE_ARGUMENT(bool, Animate)

		SLATE_ARGUMENT(int32, SpriteIndex)

		SLATE_ATTRIBUTE(const UPaperFlipbook*, Flipbook)

		SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)

		SLATE_ATTRIBUTE(TOptional<FVector2D>, CustomDesiredScale)

		SLATE_ATTRIBUTE(FVector2D, MirrorScale)

	SLATE_END_ARGS()

	SVisualImage();

	void Construct(const FArguments& Args);

	void UpdateSequence();

	void SetAnimate(bool IsAnimated);

	void SetSpriteIndex(int32 Index);

	void SetFlipbook(UPaperFlipbook* InFlipbook);

	void SetFlipbook(TAttribute<const UPaperFlipbook*> InFlipbook);

	void SetColorAndOpacity(TAttribute<FSlateColor> InColorAndOpacity);

	void SetColorAndOpacity(const FLinearColor& InLinearColor);

	void SetColorAndOpacity(const FSlateColor& InSlateColor);

	void SetDesiredScale(TAttribute<TOptional<FVector2D>> InDesiredScale);

	void SetDesiredScale(TOptional<FVector2D> InDesiredScale);

	void SetDesiredScale(const FVector2D& InDesiredScale);

	void SetMirrorScale(TAttribute<FVector2D> InMirrorScale);

	void SetMirrorScale(const FVector2D& InMirrorScale);

	void SetMirrorScale(const FScale2D& InMirrorScale);

	UPaperSprite* GetCurrentSprite() const;
	
#if WITH_ACCESSIBILITY
	virtual TSharedRef<FSlateAccessibleWidget> CreateAccessibleWidget() override;
#endif

protected:
	virtual FVector2D ComputeDesiredSize(float) const override;

	virtual bool ComputeVolatility() const override;

	FORCEINLINE FCurveSequence* GetCurveSequence() { return &CurveSequence; }

	FORCEINLINE TSlateAttributeRef<const UPaperFlipbook*> GetFlipbook() const { return TSlateAttributeRef<const UPaperFlipbook*>(AsShared(), Flipbook); }

	FORCEINLINE TSlateAttributeRef<FSlateColor> GetColorAndOpacity() const { return TSlateAttributeRef<FSlateColor>(AsShared(), ColorAndOpacity); }

	FORCEINLINE TSlateAttributeRef<TOptional<FVector2D>> GetDesiredScale() const { return TSlateAttributeRef<TOptional<FVector2D>>(AsShared(), CustomDesiredScale); }

	FORCEINLINE TSlateAttributeRef<FVector2D> GetMirrorScale() const { return TSlateAttributeRef<FVector2D>(AsShared(), MirrorScale); }

	FORCEINLINE bool GetAnimate() const { return bAnimate; }

	FORCEINLINE int32 GetSpriteIndex() const { return SpriteIndex; }

public:
	///Declared as virtual so subclasses can override them, but these methods are statically dispatched in the base class and all must be implemented
	
	///ConvertToBrush implementations
	virtual bool IsResourceValid() const;

	virtual UObject* GetFinalResource() const;

	virtual const FVector2D GetImageSize() const;
	///~ConvertToBrush implementations

	///OnPaint implementations
	virtual const FLinearColor GetFinalColorAndOpacity(const FWidgetStyle& InWidgetStyle) const;

	virtual void PreSlateDrawElementExtension() const;

	virtual FGeometry MakeCustomGeometry(const FGeometry& AllotedGeometry) const;
	///~OnPaint implementations

	virtual void PostSlateDrawElementExtension() const;

private:
	FCurveSequence CurveSequence;

	TSlateAttribute<const UPaperFlipbook*> Flipbook;

	TSlateAttribute<FSlateColor> ColorAndOpacity;

	TSlateAttribute<TOptional<FVector2D>> CustomDesiredScale;

	TSlateAttribute<FVector2D> MirrorScale;

	bool bAnimate;

	int32 SpriteIndex;
};
