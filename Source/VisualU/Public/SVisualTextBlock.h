// Copyright (c) Sam Bloomberg

#pragma once

#include "Widgets/Text/SRichTextBlock.h"

/**
* Auxiliary underlying slate widget for UVisualTextBlock
* 
* @seealso UVisualTextBlock
*/
class VISUALU_API SVisualTextBlock : public SRichTextBlock
{
public:
	/**
	* Constructor call for slate declarative syntax.
	*
	* @param Args slate arguments
	*/
	SVisualTextBlock();

	/**
	* Produces text attribute based on both TypedText and FinalText
	* 
	* @param TypedText text that appears to be typed by typewriter
	* @param FinalText full text after typewriter finishes
	* @return text attribute
	*/
	TAttribute<FText> MakeTextAttribute(const FText& TypedText, const FText& FinalText) const;

protected:
	/**
	* Caches desired size of the text.
	* 
	* @param LayoutScaleMultiplier multiplier of layout size
	*/
	virtual void CacheDesiredSize(float LayoutScaleMultiplier) override;

	/**
	* Calculates desired size.
	* 
	* @param LayoutScaleMultiplier multiplier of layout size
	* @return desired size of this widget
	*/
	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;

private:
	/**
	* Internal function responsible for making text attribute.
	* 
	* @param TypedText text that appears to be typed by typewriter
	* @param FinalText full text after typewriter finishes
	* @return typed or final text based on desired size
	*/
	FText GetTextInternal(FText TypedText, FText FinalText) const;

	/**
	* Is this widget calculating desired size of the text.
	*/
	mutable bool bIsComputingDesiredSize;

	/**
	* Desired size of this widget.
	*/
	FVector2D CachedDesiredSize;
};
