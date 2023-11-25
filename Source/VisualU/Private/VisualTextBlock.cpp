// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualTextBlock.h"
#include "VisualUSettings.h"
#include "Modules/ModuleManager.h"
#include "VisualU.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "Components/PanelSlot.h"
#include "TimerManager.h"

#define LOCTEXT_NAMESPACE "Visual U"

UVisualTextBlock::UVisualTextBlock(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer),
	LineWidth(2),
	CurrentString(),
	TextString(),
	TextLength(0),
	CurrCharCnt(0),
	bIsAppearingText(false),
	bDisplayInstantly(false)

{
	VisualUSettings = GetDefault<UVisualUSettings>();
}

void UVisualTextBlock::SetText(const FText& InText)
{
	ensureMsgf(LineWidth > 1, TEXT("Line Width is %d, text will be displayed immediately"), LineWidth);

	/*Check if this FText-to-FString conversion works properly with localization. If not, consider using FTextStringHelper*/
	TextString = InText.ToString();
	TextLength = TextString.Len();
	CurrCharCnt = 0;

	/**If something is disrupted, display text immediately*/
	if (bIsAppearingText || bDisplayInstantly || CharacterAppearanceDelay <= 0.0f || LineWidth <= 1)
	{
		FText TextToDisplay;
		const TCHAR* Buffer = TextString.GetCharArray().GetData();
		TextToDisplay = FText::FromString(Buffer);
		GetWorld()->GetTimerManager().ClearTimer(CharacterDelayTimer);
		Super::SetText(TextToDisplay);
		bIsAppearingText = false;
		return;
	}

	/**Pre-wrap text in advance for proper runtime appearance*/
	const TCHAR newline = 10;
	const TCHAR whitespace = 32;
	for (int32 i = 1; i < TextString.GetCharArray().Num(); i++)
	{
		if (i % LineWidth == 0)
		{
			int32 localCharIndex = i;
			while (TextString.IsValidIndex(localCharIndex) && TextString[localCharIndex] != whitespace)
			{
				localCharIndex--;
			}
			TextString.InsertAt(localCharIndex, newline);
			TextString.RemoveAt(localCharIndex + 1);
		}
	}

	/**Display one character of the text on the screen*/
	CharacterDelayDelegate.BindUFunction(this, TEXT("DisplayOneCharacter"));

	if (TextString.IsValidIndex(CurrCharCnt))
	{
		GetWorld()->GetTimerManager().SetTimer(CharacterDelayTimer, CharacterDelayDelegate, CharacterAppearanceDelay, true);
		bIsAppearingText = true;
	}
}

void UVisualTextBlock::SetLineWidth(int InLineWidth)
{
	LineWidth = InLineWidth;
}

void UVisualTextBlock::SetDisplayMode(bool ShouldDisplayInstantly)
{
	bDisplayInstantly = ShouldDisplayInstantly;
}

bool UVisualTextBlock::PauseTextDisplay()
{
	bool bIsTimerActive = false;

	if (GetWorld()->GetTimerManager().IsTimerActive(CharacterDelayTimer))
	{
		GetWorld()->GetTimerManager().PauseTimer(CharacterDelayTimer);
		
		bIsTimerActive = true;
	}

	return bIsTimerActive;
}

void UVisualTextBlock::UnPauseTextDisplay()
{
	GetWorld()->GetTimerManager().UnPauseTimer(CharacterDelayTimer);
}

void UVisualTextBlock::SetCharacterAppearanceDelay(float Delay)
{
	CharacterAppearanceDelay = Delay;
}

#if WITH_EDITOR
const FText UVisualTextBlock::GetPaletteCategory()
{
	return LOCTEXT("Visual U", "Visual U");
}
#endif

void UVisualTextBlock::DisplayOneCharacter()
{
	FText TextToDisplay;
	CheckForActions();
	const TCHAR ch = TextString[CurrCharCnt];
	CurrentString.AppendChar(ch);
	CurrCharCnt++;
	const TCHAR* Buffer = CurrentString.GetCharArray().GetData();
	TextToDisplay = FText::FromString(Buffer);
	Super::SetText(TextToDisplay);
	if (CurrCharCnt == TextLength)
	{
		GetWorld()->GetTimerManager().ClearTimer(CharacterDelayTimer);
		bIsAppearingText = false;
	}
}

void UVisualTextBlock::CheckForActions()
{
	const TCHAR PairCharacter = VisualUSettings->PairCharacter[0];
	if (TextString[CurrCharCnt] == PairCharacter)
	{
		ensureMsgf(TextString.IsValidIndex(CurrCharCnt + 2), TEXT("invalid use of metacharacters at char %d"), CurrCharCnt);
		const TCHAR meta = TextString[CurrCharCnt + 1];
		const FString MetaKey = FString::Printf(TEXT("%c"), meta);
		ensureMsgf(VisualUSettings->Actions.Contains(MetaKey), TEXT("Provided metacharacter \"%c\" is unspecified"), meta);
		const EVisualTextAction* Action = VisualUSettings->Actions.Find(MetaKey);
		OnActionEncountered.Broadcast(*Action);

		TextString.RemoveAt(CurrCharCnt, 3);
	}
}

#undef LOCTEXT_NAMESPACE
