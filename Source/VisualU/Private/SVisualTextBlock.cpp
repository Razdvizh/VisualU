// Copyright (c) Sam Bloomberg

#include "SVisualTextBlock.h"

SVisualTextBlock::SVisualTextBlock() = default;

TAttribute<FText> SVisualTextBlock::MakeTextAttribute(const FText& TypedText, const FText& FinalText) const
{
	return TAttribute<FText>::CreateRaw(this, &SVisualTextBlock::GetTextInternal, TypedText, FinalText);
}

FVector2D SVisualTextBlock::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	return CachedDesiredSize;
}

void SVisualTextBlock::CacheDesiredSize(float LayoutScaleMultiplier)
{
	// calculate actual maxmimum dialogue size
	bIsComputingDesiredSize = true;
	CachedDesiredSize = SRichTextBlock::ComputeDesiredSize(LayoutScaleMultiplier);
	bIsComputingDesiredSize = false;

	// poke the method again because this internally caches some junk pertaining to layout/content
	(void)SRichTextBlock::ComputeDesiredSize(LayoutScaleMultiplier);

	SRichTextBlock::CacheDesiredSize(LayoutScaleMultiplier);
}

FText SVisualTextBlock::GetTextInternal(FText TypedText, FText FinalText) const
{
	return bIsComputingDesiredSize ? FinalText : TypedText;
}
