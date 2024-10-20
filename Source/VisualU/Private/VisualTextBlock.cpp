// Copyright (c) Sam Bloomberg

#include "VisualTextBlock.h"
#include "Engine/Font.h"
#include "Engine/World.h"
#include "Styling/SlateStyle.h"
#include "BreakVisualTextBlockDecorator.h"
#include "TimerManager.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "Framework/Text/SlateTextRun.h"
#include "Framework/Text/RichTextMarkupProcessing.h"
#include "Framework/Text/ShapedTextCache.h"
#include "Framework/Text/RichTextLayoutMarshaller.h"
#include "Framework/Text/SlateTextLayout.h"

UVisualTextBlock::UVisualTextBlock(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	LetterPlayTime(0.025f),
	EndHoldTime(0.15f),
	TextLayout(),
	TextMarshaller(),
	CurrentLine(),
	Segments(),
	CachedSegmentText(),
	CachedLetterIndex(0),
	CurrentSegmentIndex(0),
	CurrentLetterIndex(0),
	MaxLetterIndex(0),
	bHasFinishedPlaying(true),
	LetterTimer()
{
	WrappingPolicy = ETextWrappingPolicy::AllowPerCharacterWrapping;
}

void UVisualTextBlock::Typewrite()
{
	UWorld* World = GetWorld();
	check(World);

	FTimerManager& TimerManager = World->GetTimerManager();
	TimerManager.ClearTimer(LetterTimer);

	CurrentLine = GetText();
	CurrentLetterIndex = 0;
	CachedLetterIndex = 0;
	CurrentSegmentIndex = 0;
	MaxLetterIndex = 0;
	Segments.Empty();
	CachedSegmentText.Empty();

	SetText(FText::GetEmpty());

	if (CurrentLine.IsEmpty())
	{
		bHasFinishedPlaying = true;
		OnTypewriterFinished();

		SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
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

	SetText(FText::FromString(CalculateSegments()));

	bHasFinishedPlaying = true;
	OnTypewriterFinished();
}

TSharedRef<SWidget> UVisualTextBlock::RebuildWidget()
{
	// Copied from URichTextBlock::RebuildWidget
	UpdateStyleData();

	TArray<TSharedRef<class ITextDecorator>> CreatedDecorators;
	CreateDecorators(CreatedDecorators);

	TextMarshaller = FRichTextLayoutMarshaller::Create(CreateMarkupParser(), CreateMarkupWriter(), CreatedDecorators, StyleInstance.Get());
	TextMarshaller->AppendInlineDecorator(MakeShared<FBreakVisualTextBlockDecorator>(this));

	MyRichTextBlock =
		SNew(SRichTextBlock)
		.TextStyle(bOverrideDefaultStyle ? &DefaultTextStyleOverride : &DefaultTextStyle)
		.Marshaller(TextMarshaller)
		.CreateSlateTextLayout(
			FCreateSlateTextLayout::CreateWeakLambda(this, [this](SWidget* InOwner, const FTextBlockStyle& InDefaultTextStyle) mutable
			{
				TextLayout = FSlateTextLayout::Create(InOwner, InDefaultTextStyle);
				return StaticCastSharedPtr<FSlateTextLayout>(TextLayout).ToSharedRef();
			}));
	
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
		SetText(FText::FromString(WrappedString));

		OnPlayLetter();
		++CurrentLetterIndex;
	}
	else
	{
		SetText(FText::FromString(CalculateSegments()));

		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearTimer(LetterTimer);

		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &ThisClass::ForceTypewriteToEnd);

		TimerManager.SetTimer(LetterTimer, Delegate, EndHoldTime, false);
	}
}

void UVisualTextBlock::CalculateWrappedString()
{
	if (TextLayout.IsValid())
	{
		const FGeometry& TextBoxGeometry = GetCachedGeometry();
		const FVector2D TextBoxSize = TextBoxGeometry.GetLocalSize();

		TextLayout->SetWrappingWidth(TextBoxSize.X);
		TextMarshaller->SetText(CurrentLine.ToString(), *TextLayout.Get());
		TextLayout->UpdateIfNeeded();

		bool bHasWrittenText = false;
		for (const FTextLayout::FLineView& View : TextLayout->GetLineViews())
		{
			const FTextLayout::FLineModel& Model = TextLayout->GetLineModels()[View.ModelIndex];

			for (TSharedRef<ILayoutBlock> Block : View.Blocks)
			{
				TSharedRef<IRun> Run = Block->GetRun();

				FDialogueTextSegment Segment;
				Run->AppendTextTo(Segment.Text, Block->GetTextRange());

				// HACK: For some reason image decorators (and possibly other decorators that don't
				// have actual text inside them) result in the run containing a zero width space instead of
				// nothing. This messes up our checks for whether the text is empty or not, which doesn't
				// have an effect on image decorators but might cause issues for other custom ones.
				if (Segment.Text.Len() == 1 && Segment.Text[0] == 0x200B)
				{
					Segment.Text.Empty();
				}

				Segment.RunInfo = Run->GetRunInfo();
				Segments.Add(Segment);

				// A segment with a named run should still take up time for the typewriter effect.
				MaxLetterIndex += FMath::Max(Segment.Text.Len(), Segment.RunInfo.Name.IsEmpty() ? 0 : 1);

				if (!Segment.Text.IsEmpty() || !Segment.RunInfo.Name.IsEmpty())
				{
					bHasWrittenText = true;
				}
			}
			
			if (bHasWrittenText)
			{
				Segments.Add(FDialogueTextSegment{ LINE_TERMINATOR });
				++MaxLetterIndex;
			}
		}

		TextLayout->SetWrappingWidth(0);
		SetText(GetText());
	}
	else
	{
		Segments.Add(FDialogueTextSegment{ CurrentLine.ToString() });
		MaxLetterIndex = Segments[0].Text.Len();
	}
}

FString UVisualTextBlock::CalculateSegments()
{
	FString Result = CachedSegmentText;

	int32 Idx = CachedLetterIndex;
	while (Idx <= CurrentLetterIndex && CurrentSegmentIndex < Segments.Num())
	{
		const FDialogueTextSegment& Segment = Segments[CurrentSegmentIndex];
		if (!Segment.RunInfo.Name.IsEmpty())
		{
			Result += FString::Printf(TEXT("<%s"), *Segment.RunInfo.Name);

			if (!Segment.RunInfo.MetaData.IsEmpty())
			{
				for (const TTuple<FString, FString>& MetaData : Segment.RunInfo.MetaData)
				{
					Result += FString::Printf(TEXT(" %s=\"%s\""), *MetaData.Key, *MetaData.Value);
				}
			}

			if (Segment.Text.IsEmpty())
			{
				Result += TEXT("/>");
				++Idx; // This still takes up an index for the typewriter effect.
			}
			else
			{
				Result += TEXT(">");
			}
		}

		bool bIsSegmentComplete = true;
		if (!Segment.Text.IsEmpty())
		{
			int32 LettersLeft = CurrentLetterIndex - Idx + 1;
			bIsSegmentComplete = LettersLeft >= Segment.Text.Len();
			LettersLeft = FMath::Min(LettersLeft, Segment.Text.Len());
			Idx += LettersLeft;

			Result += Segment.Text.Mid(0, LettersLeft);

			if (!Segment.RunInfo.Name.IsEmpty())
			{
				Result += TEXT("</>");
			}
		}

		if (bIsSegmentComplete)
		{
			CachedLetterIndex = Idx;
			CachedSegmentText = Result;
			++CurrentSegmentIndex;
		}
		else
		{
			break;
		}
	}

	return Result;
}
