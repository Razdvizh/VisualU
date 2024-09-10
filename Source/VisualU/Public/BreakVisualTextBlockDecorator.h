// Copyright (c) 2024 Evgeny Shustov

#pragma once

#include "CoreMinimal.h"
#include "Engine/TimerHandle.h"
#include "Components/RichTextBlockDecorator.h"

class UVisualTextBlock;
class URichTextBlock;

/**
* Pauses typewriter when it hits break tag - {@code <b/>}.
* Understands *text*<b>*text*</> and *text*<b/>*text*.
* Only one tag per text is supported at this time.
* 
* @note Only the last found break tag in the text will have an effect.
*/
class VISUALU_API FBreakVisualTextBlockDecorator : public FRichTextDecorator
{

typedef FRichTextDecorator Super;

public:
	FBreakVisualTextBlockDecorator(UVisualTextBlock* InOwner);

	virtual ~FBreakVisualTextBlockDecorator() override;

	/**
	* Determines the tag that is supported by this decorator.
	* 
	* @param RunParseResult contains data about parsed tags and its metadata
	* @param Text Content of Rich text block that has this decorator
	*/
	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override;

protected:
	/**
	* Produces decorated result.
	* Responsible for pausing typewriter, leaves provided text style intact.
	* 
	* @param RunInfo contains data about parsed tag and its metadata
	* @param InOutTextStyle text style to be decorated. Left as provided.
	* @param InOutString text to be modified. Left as provided.
	*/
	virtual void CreateDecoratorText(const FTextRunInfo& RunInfo, FTextBlockStyle& InOutTextStyle, FString& InOutString) const override;

private:
	/**
	* Parent constructor is hidden because we want to construct only 
	* from UVisualTextBlock
	*/
	FBreakVisualTextBlockDecorator(URichTextBlock* InOwner);

	/**
	* Timer to pause typewriter.
	*/
	mutable FTimerHandle BreakTimer;

	/**
	* Position of the break tag in the text.
	*/
	mutable int32 TagPosition;

	/**
	* Determines if at least one break tag were found.
	*/
	mutable uint32 bTagDiscovered : 1;
};
