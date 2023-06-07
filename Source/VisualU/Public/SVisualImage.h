// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation\CurveSequence.h"
#include "Widgets\SLeafWidget.h"

class UPaperFlipbook;
class UPaperSprite;

/**
 * 
 */
class VISUALU_API SVisualImage : public SLeafWidget
{
	SLATE_DECLARE_WIDGET(SVisualImage, SLeafWidget)

public:
	SLATE_BEGIN_ARGS(SVisualImage) : _Animate(false), _SpriteIndex(0) {}

		SLATE_ARGUMENT(bool, Animate)

		SLATE_ARGUMENT(int, SpriteIndex)

		SLATE_ATTRIBUTE(const UPaperFlipbook*, Flipbook)

		SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)

		SLATE_ATTRIBUTE(TOptional<FVector2D>, CustomDesiredScale)

		SLATE_ATTRIBUTE(FVector2D, MirrorScale)

	SLATE_END_ARGS()

	SVisualImage();

	void Construct(const FArguments& Args);

	void UpdateSequence();

	void SetAnimate(bool IsAnimated);

	void SetSpriteIndex(int Index);

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

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

#if WITH_ACCESSIBILITY
	virtual TSharedRef<FSlateAccessibleWidget> CreateAccessibleWidget() override;
#endif

protected:
	/** Converts currently displayed sprite to a SlateBrush and returns it*/
	FSlateBrush ConvertFlipbookToBrush() const;

	virtual FVector2D ComputeDesiredSize(float) const override;

	virtual bool ComputeVolatility() const override;

	FORCEINLINE TSlateAttributeRef<const UPaperFlipbook*> GetFlipbook() const { return TSlateAttributeRef<const UPaperFlipbook*>(AsShared(), Flipbook); }

	FORCEINLINE TSlateAttributeRef<FSlateColor> GetColorAndOpacity() const { return TSlateAttributeRef<FSlateColor>(AsShared(), ColorAndOpacity); }

	FORCEINLINE TSlateAttributeRef<TOptional<FVector2D>> GetDesiredScale() const { return TSlateAttributeRef<TOptional<FVector2D>>(AsShared(), CustomDesiredScale); }

	FORCEINLINE TSlateAttributeRef<FVector2D> GetMirrorScale() const { return TSlateAttributeRef<FVector2D>(AsShared(), MirrorScale); }

	FORCEINLINE bool GetAnimate() const { return bAnimate; }

	FORCEINLINE int GetSpriteIndex() const { return SpriteIndex; }

private:
	FCurveSequence CurveSequence;

	TSlateAttribute<const UPaperFlipbook*> Flipbook;

	TSlateAttribute<FSlateColor> ColorAndOpacity;

	TSlateAttribute<TOptional<FVector2D>> CustomDesiredScale;

	TSlateAttribute<FVector2D> MirrorScale;

	bool bAnimate;

	int SpriteIndex;
};
