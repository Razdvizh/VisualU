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

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/RichTextBlock.h"
#include "Framework/Text/RichTextLayoutMarshaller.h"
#include "Framework/Text/SlateTextLayout.h"
#include "VisualTextBlock.generated.h"

struct VISUALU_API FDialogueTextSegment
{
	FString Text;
	FTextRunParseResults RunInfo;
};

/**
 * A text block that exposes more information about text layout.
 */
UCLASS()
class VISUALU_API UVisualTextBlock : public URichTextBlock
{
	GENERATED_BODY()

public:
	UVisualTextBlock(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	FORCEINLINE TSharedPtr<IRichTextMarkupParser> GetTextParser() const
	{
		return TextParser;
	}

	// The amount of time between printing individual letters (for the "typewriter" effect).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Text Block")
	float LetterPlayTime;

	// The amount of time to wait after finishing the line before actually marking it completed.
	// This helps prevent accidentally progressing dialogue on short lines.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Text Block")
	float EndHoldTime;

	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	void PlayLine(const FText& InLine);

	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	void Pause();

	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	void Resume();

	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	void GetCurrentLine(FText& OutLine) const { OutLine = CurrentLine; }

	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	bool HasFinishedPlayingLine() const { return bHasFinishedPlaying; }

	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	void SkipToLineEnd();

	// variants to feed slate widget more info
	virtual void SetTextPartiallyTyped(const FText& InText, const FText& InFinalText);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Visual Text Block")
	void OnPlayLetter();

	UFUNCTION(BlueprintImplementableEvent, Category = "Visual Text Block")
	void OnLineFinishedPlaying();

	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	void PlayNextLetter();

	void CalculateWrappedString();

	FString CalculateSegments();

	TSharedPtr<IRichTextMarkupParser> TextParser;

	UPROPERTY()
	FText CurrentLine;

	TArray<FDialogueTextSegment> Segments;

	// The section of the text that's already been printed out and won't ever change.
	// This lets us cache some of the work we've already done. We can't cache absolutely
	// everything as the last few characters of a string may change if they're related to
	// a named run that hasn't been completed yet.
	FString CachedSegmentText;

	int32 CurrentSegmentIndex;

	int32 CurrentLetterIndex;

	int32 MaxLetterIndex;

	uint32 bHasFinishedPlaying : 1;

	FTimerHandle LetterTimer;
};
