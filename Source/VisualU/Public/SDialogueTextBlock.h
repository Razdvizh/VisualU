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

#pragma once

#include "Widgets/Text/SRichTextBlock.h"

class VISUALU_API SDialogueTextBlock : public SRichTextBlock
{
public:
	SDialogueTextBlock();

	TAttribute<FText> MakeTextAttribute(const FText& TypedText, const FText& FinalText) const;

protected:
	virtual void CacheDesiredSize(float LayoutScaleMultiplier) override;

	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;

private:
	FText GetTextInternal(FText TypedText, FText FinalText) const;

	mutable bool bIsComputingDesiredSize;

	FVector2D CachedDesiredSize;
};
