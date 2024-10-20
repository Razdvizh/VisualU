// Copyright (c) 2024 Evgeny Shustov


#include "BreakVisualTextBlockDecorator.h"
#include "Components/RichTextBlock.h"
#include "Styling/SlateTypes.h"
#include "VisualTextBlock.h"
#include "TimerManager.h"

FBreakVisualTextBlockDecorator::FBreakVisualTextBlockDecorator(UVisualTextBlock* InOwner)
	: Super(InOwner),
	BreakTimer(),
	TagPosition(0),
	bTagDiscovered(false)
{
}

bool FBreakVisualTextBlockDecorator::Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const
{
	if (RunParseResult.Name == TEXT("b"))
	{
		int32 TagCharsCnt = 0;
		int32 CurrentIndex = RunParseResult.OriginalRange.BeginIndex;
		while ((CurrentIndex = Text.Find(TEXT("/>"), ESearchCase::IgnoreCase, ESearchDir::FromEnd, CurrentIndex)) != INDEX_NONE)
		{
			int32 NumChars = 0;
			int32 TagStartIndex = Text.Find(TEXT("<"), ESearchCase::IgnoreCase, ESearchDir::FromEnd, CurrentIndex);

			const bool bIsEnclosingTag = TagStartIndex + 1 == CurrentIndex;
			if (bIsEnclosingTag)
			{
				const int32 StyleTagEnd = Text.Find(TEXT(">"), ESearchCase::IgnoreCase, ESearchDir::FromEnd, TagStartIndex);
				TagStartIndex = Text.Find(TEXT("<"), ESearchCase::IgnoreCase, ESearchDir::FromEnd, StyleTagEnd);
				//four is the number of metacharacters
				NumChars = (StyleTagEnd - TagStartIndex) + 4;
			}
			else
			{
				//two is the number of metacharacters
				NumChars = (CurrentIndex - TagStartIndex) + 2;
			}

			//Found metadata which means tag is replaced with something, consider it a character
			if (Text.Find(TEXT("=\""), ESearchCase::IgnoreCase, ESearchDir::FromStart, TagStartIndex) != INDEX_NONE)
			{
				NumChars--;
			}

			check(NumChars >= 0);
			TagCharsCnt += NumChars;
			CurrentIndex -= 2;
		}
		TagPosition = RunParseResult.OriginalRange.BeginIndex - TagCharsCnt + 1;
		return true;
	}

	return false;
}

void FBreakVisualTextBlockDecorator::CreateDecoratorText(const FTextRunInfo& RunInfo, FTextBlockStyle& InOutTextStyle, FString& InOutString) const
{
	if (ensure(TagPosition > 0))
	{
		UVisualTextBlock* VisualTextBlock = ExactCast<UVisualTextBlock>(Owner);
		check(VisualTextBlock);

		UWorld* World = VisualTextBlock->GetWorld();
		check(World);

		FTimerManager& TimerManager = World->GetTimerManager();

		if (!bTagDiscovered)
		{
			const float BreakDelay = TagPosition * VisualTextBlock->LetterPlayTime;
			TimerManager.SetTimer(BreakTimer, FTimerDelegate::CreateUObject(VisualTextBlock, &UVisualTextBlock::Pause), BreakDelay, /*InbLoop*/false);
			bTagDiscovered = true;
		}
	}
}

FBreakVisualTextBlockDecorator::FBreakVisualTextBlockDecorator(URichTextBlock* InOwner)
	: Super(InOwner),
	BreakTimer(),
	TagPosition(0),
	bTagDiscovered(false)
{
}