// Copyright (c) 2024 Evgeny Shustov
// Copyright (c) 2024 Sam Bloomberg

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/RichTextBlock.h"
#include "Framework/Text/IRichTextMarkupParser.h"
#include "VisualTextBlock.generated.h"

class FSlateTextLayout;
class FRichTextLayoutMarshaller;

/**
* Data that represents text segment.
*/
struct VISUALU_API FDialogueTextSegment
{
	FString Text;
	FRunInfo RunInfo;
};

/**
 * A text block that exposes more information about text layout.
 * Supports break tag that pauses typewriter when it is encountered in text.
 * 
 * @see FBreakVisualTextBlockDecorator
 */
UCLASS()
class VISUALU_API UVisualTextBlock : public URichTextBlock
{
	GENERATED_BODY()

public:
	UVisualTextBlock(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	* The amount of time, in seconds, between printing individual letters.
	* during for the typewriter effect.
	* Zero means no typewriter effect.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Text Block", meta = (UIMin = 0.f, ClampMin = 0.f))
	float LetterPlayTime;

	/**
	* The amount of time, in seconds, to wait after finishing the line
	* before actually marking it completed. This helps prevent 
	* accidentally progressing dialogue on short lines.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Text Block", meta = (UIMin = 0.f, ClampMin = 0.f))
	float EndHoldTime;

	/**
	* Applies typewriter effect to the current text of the text block.
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	void Typewrite();

	/**
	* Pauses active typewriter effect.
	* 
	* @note not threadsafe, has no effect for inactive timer.
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	void Pause();

	/**
	* Resumes active typewriter effect.
	* 
	* @note not threadsafe, has no effect for active timer.
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	void Resume();

	/**
	* @return {@code true} for stopped typewriter
	* 
	* @see UVisualTextBlock::bHasFinishedPlaying
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	FORCEINLINE bool HasTypewriterFinished() const { return bHasFinishedPlaying; }

	/**
	* Unconditionally stops typewriter effect.
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	void ForceTypewriteToEnd();

protected:
	/**
	* Called when individual letter is typed by typewriter.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Visual Text Block")
	void OnPlayLetter();

	/**
	* Called when typewriter effect is finished.
	* 
	* @note pausing typewriter will not trigger this event
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Visual Text Block")
	void OnTypewriterFinished();

	/**
	* @return underlying slate widget
	*/
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	/**
	* Displays next letter during typewriter effect. 
	*/
	void PlayNextLetter();

	/**
	* Wraps in advance UVisualTextBlock::CurrentLine before it is typed.
	*/
	void CalculateWrappedString();

	/**
	* @return parsed dialogue segments as string
	*/
	FString CalculateSegments();

private:
	/**
	* Text layout of this text block.
	*/
	TSharedPtr<FSlateTextLayout> TextLayout;

	/**
	* Text marshaller for the text layout.
	*/
	TSharedPtr<FRichTextLayoutMarshaller> TextMarshaller;

	/**
	* Currently typed text.
	*/
	UPROPERTY()
	FText CurrentLine;

	/**
	* Processed dialog text segments.
	*/
	TArray<FDialogueTextSegment> Segments;

	/**
	* The section of the text that's already been printed
	* out and won't ever change. This lets us cache some of the work
	* we've already done. We can't cache absolutely everything
	* as the last few characters of a string may change if they're related to
	* a named run that hasn't been completed yet.
	*/
	FString CachedSegmentText;

	/**
	* Index of last letter that already been printed out and won't ever change.
	*/
	int32 CachedLetterIndex;

	/**
	* Current position of a segment.
	*/
	int32 CurrentSegmentIndex;

	/**
	* Current position of a letter.
	*/
	int32 CurrentLetterIndex;

	/**
	* Most reachable position for a letter.
	*/
	int32 MaxLetterIndex;

	/**
	* State of the typewriter effect.
	*/
	uint32 bHasFinishedPlaying : 1;

	/**
	* Timer used to play letters as in typewriter.
	*/
	FTimerHandle LetterTimer;
};
