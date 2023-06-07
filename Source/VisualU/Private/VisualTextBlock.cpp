// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualTextBlock.h"
#include "Framework/Application/SlateApplication.h"
#include "Fonts/FontMeasure.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "Components/PanelSlot.h"
#include "TimerManager.h"

#define LOCTEXT_NAMESPACE "Visual U"

UVisualTextBlock::UVisualTextBlock(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UVisualTextBlock::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	TArray<FRichTextStyleRow*> Rows;
	FSlateFontInfo Font;

	if (TextStyleSet && TextStyleSet->GetRowStruct()->IsChildOf(FRichTextStyleRow::StaticStruct()))
	{
		TextStyleSet.Get()->GetAllRows(TEXT("VisualTextBlock"), Rows);
		Font = Rows[0]->TextStyle.Font;
	}
	else if (bOverrideDefaultStyle)
	{
		Font = DefaultTextStyleOverride.Font;
	}
	
	FontSize = FSlateApplication::Get().GetRenderer()->GetFontMeasureService()->Measure(Text, Font, 1.f);
}

void UVisualTextBlock::SetText(const FText& InText)
{
	ensureMsgf(LineWidth > 1, TEXT("Line Width is %d, text will be displayed immediately"), LineWidth);
	 
	/*Check if this FText-to-FString conversion works properly with localization. If not, consider using FTextStringHelper*/
	CurrentString = FString();
	TextString = InText.ToString();
	TextLength = TextString.Len();
	CurrCharCnt = 0;

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

#undef LOCTEXT_NAMESPACE
