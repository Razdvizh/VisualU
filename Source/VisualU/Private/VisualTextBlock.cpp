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

#include "VisualTextBlock.h"
#include "Engine/Font.h"
#include "Engine/World.h"
#include "Styling/SlateStyle.h"
#include "SVisualTextBlock.h"
#include "TimerManager.h"
#include "Framework/Text/SlateTextRun.h"
#include "Framework/Text/RichTextMarkupProcessing.h"
#include "Framework/Text/ShapedTextCache.h"

/**
 * Text run that represents a segment of text which is in the process of being typed out.
 * The size of the text block will represent the final size of each word rather than the provided content.
 */
class FPartialDialogueRun : public FSlateTextRun
{
public:
	FPartialDialogueRun(const FRunInfo& InRunInfo, 
		const TSharedRef<const FString>& InText, 
		const FTextBlockStyle& InStyle, 
		const FTextRange& InRange, 
		const FDialogueTextSegment& Segment)
		: FSlateTextRun(InRunInfo, InText, InStyle, InRange),
		Segment(Segment)
	{
	}

	virtual FVector2D Measure(int32 StartIndex, int32 EndIndex, float Scale, const FRunTextContext& TextContext) const override
	{
		if (EndIndex != Range.EndIndex)
		{
			// measuring text within existing range, refer to normal implementation
			return FSlateTextRun::Measure(StartIndex, EndIndex, Scale, TextContext);
		}
		else
		{
			// attempting to measure to end of typed range, construct future typed content from source segment and measure based on that instead.
			// this will ensure text is wrapped prior to being fully typed.
			const FString CombinedContent = ConstructCombinedText();
			return MeasureInternal(StartIndex, CombinedContent.Len(), Scale, TextContext, CombinedContent);
		}
	}

private:
	FString ConstructCombinedText() const
	{
		const int32 ExistingChars = Range.Len();

		FString FutureContent;
		if (!Segment.RunInfo.ContentRange.IsEmpty())
		{
			// with tags
			const int32 SubstringStart = Segment.RunInfo.ContentRange.BeginIndex - Segment.RunInfo.OriginalRange.BeginIndex + ExistingChars;
			const int32 NumChars = Segment.RunInfo.ContentRange.Len() - ExistingChars;
			FutureContent = Segment.Text.Mid(SubstringStart, NumChars);
		}
		else
		{
			// no tags
			const int32 NumChars = Segment.RunInfo.OriginalRange.Len() - ExistingChars;
			FutureContent = Segment.Text.Mid(ExistingChars, Segment.RunInfo.OriginalRange.Len() - ExistingChars);
		}
		// trim to next possible wrap opportunity
		for (int32 i = 0; i < FutureContent.Len(); ++i)
		{
			const TCHAR FutureChar = FutureContent[i];
			if (FText::IsWhitespace(FutureChar))
			{
				FutureContent.LeftInline(i);
				break;
			}
		}

		return *Text + FutureContent;
	}

	FVector2D MeasureInternal(int32 BeginIndex, int32 EndIndex, float Scale, const FRunTextContext& TextContext, const FString& InText) const
	{
		const FVector2D ShadowOffsetToApply((EndIndex == Range.EndIndex) 
			? FMath::Abs(Style.ShadowOffset.X * Scale) 
			: 0.0f, FMath::Abs(Style.ShadowOffset.Y * Scale));

		// Offset the measured shaped text by the outline since the outline was not factored into the size of the text
		// Need to add the outline offsetting to the beginning and the end because it surrounds both sides.
		const float ScaledOutlineSize = Style.Font.OutlineSettings.OutlineSize * Scale;
		const FVector2D OutlineSizeToApply(
			(BeginIndex == Range.BeginIndex ? ScaledOutlineSize : 0) + (EndIndex == Range.EndIndex ? ScaledOutlineSize : 0), 
			ScaledOutlineSize);

		if (EndIndex - BeginIndex == 0)
		{
			return FVector2D(0, GetMaxHeight(Scale)) + ShadowOffsetToApply + OutlineSizeToApply;
		}

		// Use the full text range (rather than the run range) so that text that spans runs will still be shaped correctly
		return ShapedTextCacheUtil::MeasureShapedText(
			TextContext.ShapedTextCache, 
			FCachedShapedTextKey(FTextRange(0, InText.Len()), Scale, TextContext, Style.Font), 
			FTextRange(BeginIndex, EndIndex), *InText) + 
			ShadowOffsetToApply +
			OutlineSizeToApply;
	}

	const FDialogueTextSegment& Segment;
};

/**
 * A decorator that intercepts partially typed segments and allocates an FPartialDialogueRun to represent them.
 */
class FPartialDialogueDecorator : public ITextDecorator
{
public:
	FPartialDialogueDecorator(
		const TArray<FDialogueTextSegment>* Segments, 
		const int32* CurrentSegmentIndex) 
		: Segments(Segments),
		CurrentSegmentIndex(CurrentSegmentIndex)
	{
	}

	virtual bool Supports(const FTextRunParseResults& RunInfo, const FString& Text) const override
	{
		// no segments have been calculated yet
		if (*CurrentSegmentIndex >= Segments->Num())
		{
			return false;
		}

		// does this run relate to the segment which is still in-flight?
		const FDialogueTextSegment& Segment = (*Segments)[*CurrentSegmentIndex];
		const FTextRange& SegmentRange = !RunInfo.ContentRange.IsEmpty() ? Segment.RunInfo.ContentRange : Segment.RunInfo.OriginalRange;
		const FTextRange& RunRange = !RunInfo.ContentRange.IsEmpty() ? RunInfo.ContentRange : RunInfo.OriginalRange;
		const FTextRange IntersectedRange = RunRange.Intersect(SegmentRange);
		return !IntersectedRange.IsEmpty() && SegmentRange != IntersectedRange;
	}

	virtual TSharedRef<ISlateRun> Create(const TSharedRef<class FTextLayout>& TextLayout, const FTextRunParseResults& InRunInfo, const FString& ProcessedString, const TSharedRef<FString>& InOutModelText, const ISlateStyle* InStyle) override
	{
		// copied from FRichTextLayoutMarshaller::AppendRunsForText
		FRunInfo RunInfo(InRunInfo.Name);
		for (const TPair<FString, FTextRange>& Pair : InRunInfo.MetaData)
		{
			int32 Length = FMath::Max(0, Pair.Value.EndIndex - Pair.Value.BeginIndex);
			RunInfo.MetaData.Add(Pair.Key, ProcessedString.Mid(Pair.Value.BeginIndex, Length));
		}

		// resolve text style
		const bool CanParseTags = !InRunInfo.Name.IsEmpty() && InStyle->HasWidgetStyle<FTextBlockStyle>(*InRunInfo.Name);
		const FTextBlockStyle& Style = CanParseTags 
			? InStyle->GetWidgetStyle<FTextBlockStyle>(*InRunInfo.Name) 
			: StaticCast<FSlateTextLayout&>(*TextLayout).GetDefaultTextStyle();

		// skip tags if valid style parser found
		const FTextRange& Range = CanParseTags ? InRunInfo.ContentRange : InRunInfo.OriginalRange;
		FTextRange ModelRange(InOutModelText->Len(), InOutModelText->Len() + Range.Len());

		const FDialogueTextSegment& Segment = (*Segments)[*CurrentSegmentIndex];
		*InOutModelText += Segment.Text.Mid(Range.BeginIndex - Segment.RunInfo.OriginalRange.BeginIndex, Range.Len());

		return MakeShared<FPartialDialogueRun>(RunInfo, InOutModelText, Style, ModelRange, Segment);
	}

private:
	const TArray<FDialogueTextSegment>* Segments;

	const int32* CurrentSegmentIndex;
};

UVisualTextBlock::UVisualTextBlock(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	LetterPlayTime(0.025f),
	EndHoldTime(0.15f),
	CurrentLine(),
	Segments(),
	CachedSegmentText(),
	CurrentSegmentIndex(0),
	CurrentLetterIndex(0),
	MaxLetterIndex(0),
	bHasFinishedPlaying(true),
	LetterTimer()
{
}

void UVisualTextBlock::Typewrite()
{
	UWorld* World = GetWorld();
	check(World);

	FTimerManager& TimerManager = World->GetTimerManager();
	TimerManager.ClearTimer(LetterTimer);

	CurrentLine = GetText();
	CurrentLetterIndex = 0;
	CurrentSegmentIndex = 0;
	MaxLetterIndex = 0;
	Segments.Empty();
	CachedSegmentText.Empty();

	if (CurrentLine.IsEmpty())
	{
		SetText(FText::GetEmpty());

		bHasFinishedPlaying = true;
		OnTypewriterFinished();

		SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		SetTextPartiallyTyped(FText::GetEmpty(), CurrentLine);

		bHasFinishedPlaying = false;

		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &ThisClass::PlayNextLetter);

		TimerManager.SetTimer(LetterTimer, Delegate, LetterPlayTime, true);

		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UVisualTextBlock::Pause()
{
	UWorld* World = GetWorld();
	check(World);

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	if (IsInGameThread() && TimerManager.IsTimerActive(LetterTimer))
	{
		TimerManager.PauseTimer(LetterTimer);
	}
}

void UVisualTextBlock::Resume()
{
	UWorld* World = GetWorld();
	check(World);

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	if (IsInGameThread() && TimerManager.IsTimerPaused(LetterTimer))
	{
		TimerManager.UnPauseTimer(LetterTimer);
	}
}

void UVisualTextBlock::ForceTypewriteToEnd()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(LetterTimer);

	CurrentLetterIndex = MaxLetterIndex - 1;

	SetText(CurrentLine);

	bHasFinishedPlaying = true;
	OnTypewriterFinished();
}

void UVisualTextBlock::SetTextPartiallyTyped(const FText& InText, const FText& InFinalText)
{
	Super::SetText(InText);

	if (SVisualTextBlock* DialogueTextBlock = StaticCast<SVisualTextBlock*>(MyRichTextBlock.Get()))
	{
		DialogueTextBlock->SetText(DialogueTextBlock->MakeTextAttribute(InText, InFinalText));
	}
}

TSharedRef<SWidget> UVisualTextBlock::RebuildWidget()
{
	// Copied from URichTextBlock::RebuildWidget
	UpdateStyleData();

	TArray<TSharedRef<class ITextDecorator>> CreatedDecorators;
	CreateDecorators(CreatedDecorators);

	TextParser = CreateMarkupParser();
	TSharedRef<FRichTextLayoutMarshaller> Marshaller = FRichTextLayoutMarshaller::Create(TextParser, CreateMarkupWriter(), CreatedDecorators, StyleInstance.Get());
	// add custom decorator to intercept partially typed segments
	Marshaller->AppendInlineDecorator(MakeShared<FPartialDialogueDecorator>(&Segments, &CurrentSegmentIndex));
	Marshaller->AppendInlineDecorator(MakeShared<FBreakVisualTextBlockDecorator>(this));

	MyRichTextBlock =
		SNew(SVisualTextBlock)
		.TextStyle(bOverrideDefaultStyle ? &GetDefaultTextStyleOverride() : &DefaultTextStyle)
		.Marshaller(Marshaller);

	return MyRichTextBlock.ToSharedRef();
}

void UVisualTextBlock::PlayNextLetter()
{
	if (Segments.IsEmpty())
	{
		CalculateWrappedString();
	}

	FString WrappedString = CalculateSegments();

	// TODO: How do we keep indexing of text i18n-friendly?
	if (CurrentLetterIndex < MaxLetterIndex)
	{
		SetTextPartiallyTyped(FText::FromString(WrappedString), CurrentLine);

		OnPlayLetter();
		++CurrentLetterIndex;
	}
	else
	{
		SetText(CurrentLine);

		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearTimer(LetterTimer);

		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &ThisClass::ForceTypewriteToEnd);

		TimerManager.SetTimer(LetterTimer, Delegate, EndHoldTime, false);
	}
}

void UVisualTextBlock::CalculateWrappedString()
{
	if (TSharedPtr<IRichTextMarkupParser> Parser = GetTextParser(); Parser.IsValid())
	{
		TArray<FTextLineParseResults> Lines;
		FString ProcessedString;
		Parser->Process(Lines, CurrentLine.ToString(), ProcessedString);
		for (int32 LineIdx = 0; LineIdx < Lines.Num(); ++LineIdx)
		{
			const FTextLineParseResults& Line = Lines[LineIdx];
			for (const FTextRunParseResults& Run : Line.Runs)
			{
				Segments.Emplace(
					FDialogueTextSegment
					{
						ProcessedString.Mid(Run.OriginalRange.BeginIndex, Run.OriginalRange.Len()),
						Run
					});
			}

			if (LineIdx != Lines.Num() - 1)
			{
				Segments.Emplace(
					FDialogueTextSegment
					{
						TEXT("\n"),
						FTextRunParseResults(FString(), FTextRange(0, 1))
					});
				++MaxLetterIndex;
			}

			MaxLetterIndex = Line.Range.EndIndex;
		}
	}
}

FString UVisualTextBlock::CalculateSegments()
{
	while (CurrentSegmentIndex < Segments.Num())
	{
		const FDialogueTextSegment& Segment = Segments[CurrentSegmentIndex];

		int32 SegmentStartIndex = FMath::Max(Segment.RunInfo.OriginalRange.BeginIndex, Segment.RunInfo.ContentRange.BeginIndex);
		CurrentLetterIndex = FMath::Max(CurrentLetterIndex, SegmentStartIndex);

		if (Segment.RunInfo.ContentRange.IsEmpty() ? !Segment.RunInfo.OriginalRange.Contains(CurrentLetterIndex) : !Segment.RunInfo.ContentRange.Contains(CurrentLetterIndex))
		{
			CachedSegmentText += Segment.Text;
			CurrentSegmentIndex++;
			continue;
		}

		// is this segment an inline tag? eg. <blah/>
		if (!Segment.RunInfo.Name.IsEmpty() && !Segment.RunInfo.OriginalRange.IsEmpty() && Segment.RunInfo.ContentRange.IsEmpty())
		{
			// seek to end of tag - treat as single character
			int32 SegmentEndIndex = FMath::Max(Segment.RunInfo.OriginalRange.EndIndex, Segment.RunInfo.ContentRange.EndIndex);
			CurrentLetterIndex = FMath::Max(CurrentLetterIndex, SegmentEndIndex);
			return CachedSegmentText + Segment.Text;
		}
		// is this segment partially typed?
		else if (Segment.RunInfo.OriginalRange.Contains(CurrentLetterIndex))
		{
			FString Result = CachedSegmentText + Segment.Text.Mid(0, CurrentLetterIndex - Segment.RunInfo.OriginalRange.BeginIndex);

			// if content tags need closing, append the remaining tag characters
			if (!Segment.RunInfo.ContentRange.IsEmpty() && Segment.RunInfo.ContentRange.Contains(CurrentLetterIndex))
			{
				Result += Segment.Text.Mid(Segment.RunInfo.ContentRange.EndIndex - Segment.RunInfo.OriginalRange.BeginIndex, Segment.RunInfo.OriginalRange.EndIndex - Segment.RunInfo.ContentRange.EndIndex);
			}

			return Result;
		}
		break;
	}

	return CachedSegmentText;
}
