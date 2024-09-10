// Copyright (c) Sam Bloomberg

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/RichTextBlock.h"
#include "Framework/Text/IRichTextMarkupParser.h"
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

	/*
	* Applies typewriter effect to the current text of the text block.
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	void Typewrite();

	/*
	* Pause active typewriter effect.
	* @note not threadsafe, has no effect for inactive timer.
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	void Pause();

	/*
	* Resume active typewriter effect.
	* @note not threadsafe, has no effect for active timer.
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	void Resume();

	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	FORCEINLINE bool HasTypewriterFinished() const { return bHasFinishedPlaying; }

	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	void ForceTypewriteToEnd();

	// variants to feed slate widget more info
	virtual void SetTextPartiallyTyped(const FText& InText, const FText& InFinalText);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Visual Text Block")
	void OnPlayLetter();

	UFUNCTION(BlueprintImplementableEvent, Category = "Visual Text Block")
	void OnTypewriterFinished();

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
