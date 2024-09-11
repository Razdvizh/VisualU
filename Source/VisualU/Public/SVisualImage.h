// Copyright (c) 2024 Evgeny Shustov

#pragma once

#include "CoreMinimal.h"
#include "Animation/CurveSequence.h"
#include "SVisualImageBase.h"
#include "UObject/GCObject.h"

class UPaperFlipbook;
class UPaperSprite;

/**
* Slate widget that displays sprite flipbooks.
* 
* @see UVisualImage
*/
class VISUALU_API SVisualImage : public SVisualImageBase<SVisualImage>, public FGCObject
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

	/**
	* Constructor call for slate declarative syntax.
	*
	* @param Args slate arguments
	*/
	void Construct(const FArguments& Args);

	/**
	* Resets SVisualImage::CurveSequence.
	*/
	void UpdateSequence();

	/**
	* Setter for SVisualImage::bAnimate.
	* 
	* @param IsAnimated {@code true} to animate flipbook
	*/
	void SetAnimate(bool IsAnimated);

	/**
	* Setter for SVisualImage::SpriteIndex.
	* 
	* @param Index new sprite index
	*/
	void SetSpriteIndex(int32 Index);

	/**
	* Setter for SVisualImage::Flipbook.
	* 
	* @param InFlipbook new render resource for this slate widget
	*/
	void SetFlipbook(UPaperFlipbook* InFlipbook);

	/**
	* Attribute version.
	* 
	* @see SVisualImage::Flipbook(UPaperFlipbook*)
	* 
	* @param InFlipbook new paper flipbook attribute
	*/
	void SetFlipbook(TAttribute<const UPaperFlipbook*> InFlipbook);

	/**
	* Setter for SVisualImage::ColorAndOpacity.
	* 
	* @param InLinearColor new color and opacity of the flipbook
	*/
	void SetColorAndOpacity(const FLinearColor& InLinearColor);

	/**
	* Attribute version.
	* 
	* @see SVisualImage::SetColorAndOpacity(FLinearColor)
	* 
	* @param InColorAttribute new color and opacity attribute
	*/
	void SetColorAndOpacity(TAttribute<FSlateColor> InColorAttribute);

	/**
	* Slate color version.
	*
	* @see SVisualImage::SetColorAndOpacity(FLinearColor)
	*
	* @param InSlateColor slate color and opacity
	*/
	void SetColorAndOpacity(const FSlateColor& InSlateColor);

	/**
	* Setter for SVisualImage::CustomDesiredScale.
	* 
	* @param InDesiredScale new desired scale of the flipbook
	*/
	void SetDesiredScale(const FVector2D& InDesiredScale);

	/**
	* Attribute version.
	* 
	* @see SVisualImage::SetDesiredScale(const FVector2D&)
	*
	* @param InDesiredScale new desired scale attribute
	*/
	void SetDesiredScale(TAttribute<TOptional<FVector2D>> InDesiredScale);

	/**
	* TOptional version.
	* 
	* @see SVisualImage::SetDesiredScale(const FVector2D&)
	*
	* @param InDesiredScale new optional desired scale
	*/
	void SetDesiredScale(TOptional<FVector2D> InDesiredScale);

	/**
	* Setter for SVisualImage::MirrorScale.
	* 
	* @param InMirrorScale new mirror scale of the flipbook
	*/
	void SetMirrorScale(const FVector2D& InMirrorScale);

	/**
	* Attribute version.
	* 
	* @see SVisualImage::SetMirrorScale(const FVector2D&)
	* 
	* @param InMirrorScale new mirror scale attribute
	*/
	void SetMirrorScale(TAttribute<FVector2D> InMirrorScale);

	/**
	* Scale 2D version.
	* 
	* @see SVisualImage::SetMirrorScale(const FVector2D&)
	* 
	* @param InMirrorScale new mirror scale 2D
	*/
	void SetMirrorScale(const FScale2D& InMirrorScale);

	/**
	* @return sprite that is rendered at this time
	*/
	UPaperSprite* GetCurrentSprite() const;
	
#if WITH_ACCESSIBILITY
	/**
	* @return This widget as accessible widget
	*/
	virtual TSharedRef<FSlateAccessibleWidget> CreateAccessibleWidget() override;
#endif

protected:
	/**
	* Calculates desire size.
	* Considers size of the currently rendered sprite
	* and SVisualImage::CustomDesiredScale.
	* 
	* @return computed desired size of this widget
	*/
	virtual FVector2D ComputeDesiredSize(float) const override;

	/**
	* @return {@code true} when this widget is volatile (slate-wise)
	*/
	virtual bool ComputeVolatility() const override;

	/**
	* @return SVisualImage::CurveSequence
	*/
	FORCEINLINE FCurveSequence* GetCurveSequence() { return &CurveSequence; }

	/**
	* @return SVisualImage::Flipbook
	*/
	FORCEINLINE TSlateAttributeRef<const UPaperFlipbook*> GetFlipbook() const { return TSlateAttributeRef<const UPaperFlipbook*>(AsShared(), Flipbook); }

	/**
	* @return SVisualImage::ColorAndOpacity
	*/
	FORCEINLINE TSlateAttributeRef<FSlateColor> GetColorAndOpacity() const { return TSlateAttributeRef<FSlateColor>(AsShared(), ColorAndOpacity); }

	/**
	* @return SVisualImage::CustomDesiredScale
	*/
	FORCEINLINE TSlateAttributeRef<TOptional<FVector2D>> GetDesiredScale() const { return TSlateAttributeRef<TOptional<FVector2D>>(AsShared(), CustomDesiredScale); }

	/**
	* @return SVisualImage::MirrorScale
	*/
	FORCEINLINE TSlateAttributeRef<FVector2D> GetMirrorScale() const { return TSlateAttributeRef<FVector2D>(AsShared(), MirrorScale); }

	/**
	* @return SVisualImage::bAnimate
	*/
	FORCEINLINE bool GetAnimate() const { return bAnimate; }

	/**
	* @return SVisualImage::SpriteIndex
	*/
	FORCEINLINE int32 GetSpriteIndex() const { return SpriteIndex; }

	/**
	* Provides references to members for Garbage Collector.
	*
	* @param Collector gathers references to members
	*/
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	/**
	* @return Name of the owner of references
	*/
	virtual FString GetReferencerName() const override;

public:
	/**Declared as virtual so subclasses can override them, but these methods are statically dispatched in the base class and all must be implemented*/
	
	/**
	* @see SVisualImageBase::IsResourceValid()
	* 
	* @return {@code true} when flipbook is valid
	*/
	virtual bool IsResourceValid() const;

	/**
	* @see SVisualImageBase::GetFinalResource()
	* 
	* @return valid flipbook
	*/
	virtual UObject* GetFinalResource() const;

	/**
	* @see SVisualImageBase::GetImageSize()
	* 
	* @return slate size of the flipbook
	*/
	virtual const FVector2D GetImageSize() const;

	/**
	* Applies SVisualImage::ColorAndOpacity to the final color of the flipbook.
	* 
	* @see SVisualImageBase::GetFinalColorAndOpacity()
	* 
	* @param InWidgetStyle base widget style
	* @return modified color and opacity
	*/
	virtual const FLinearColor GetFinalColorAndOpacity(const FWidgetStyle& InWidgetStyle) const;

	/**
	* No extension.
	* 
	* @see SVisualImageBase::PreSlateDrawElementExtension()
	*/
	virtual void PreSlateDrawElementExtension() const;

	/**
	* Applies SVisualImage::MirrorScale to the brush geometry when it is valid.
	* 
	* @see SVisualImageBase::MakeCustomGeometry()
	* 
	* @param AllotedGeometry base geometry for this widget
	* @return modified or untouched AllotedGeometry for flipbook
	*/
	virtual FGeometry MakeCustomGeometry(const FGeometry& AllotedGeometry) const;

	/**
	* No extension.
	* 
	* @see SVisualImageBase::PostSlateDrawElementExtension()
	*/
	virtual void PostSlateDrawElementExtension() const;

private:
	/**
	* Drives animation of the flipbook.
	*/
	FCurveSequence CurveSequence;

	/**
	* Resource that is rendered by this widget.
	*/
	TSlateAttribute<const UPaperFlipbook*> Flipbook;

	/**
	* Color and opacity of the flipbook.
	* Does not override all other color and opacity modifiers.
	*/
	TSlateAttribute<FSlateColor> ColorAndOpacity;

	/**
	* Optional custom scale for the flipbook.
	*/
	TSlateAttribute<TOptional<FVector2D>> CustomDesiredScale;

	/**
	* Orientational scale of the flipbook.
	* Can be mirrored along both X and Y axes.
	* 
	* @note zero values are invalid.
	*/
	TSlateAttribute<FVector2D> MirrorScale;

	/**
	* Animation state of the flipbook.
	* When true, SVisualImage::SpriteIndex has no effect.
	*/
	bool bAnimate;

	/**
	* Index of specific sprite of the flipbook to display.
	* Must be within bounds of flipbook frames.
	* 
	* @note has no effect when SVisualImage::bAnimate is true
	*/
	int32 SpriteIndex;
};
