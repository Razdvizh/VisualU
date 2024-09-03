/*
* MIT License
*
* Copyright(c) 2021 Sam Bloomberg
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Original repository by Sam Bloomberg (@redxdev): https://github.com/redxdev/UnrealRichTextDialogueBox
* Forked repository by Adam Parkinson (@SalamiArmi): https://github.com/SalamiArmi/UnrealRichTextDialogueBox
*/

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
