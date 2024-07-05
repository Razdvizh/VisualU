// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlock.h"
#include "VisualTextBlock.generated.h"

class UVisualUSettings;
///\todo change this to regular dynamic multicast delegate
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FOnActionEncountered, UVisualTextBlock, OnActionEncountered, const EVisualTextAction, Action);

/// <summary>
/// Supports Visual Novel style of typewriter effect.
/// </summary>
/// <remarks>
/// <c>Visual Text Block</c> can be used as [Rich Text Block](https://docs.unrealengine.com/4.27/en-US/API/Runtime/UMG/Components/URichTextBlock/)
/// for static rich text and embedded images, fields that <c>Visual Text Block</c> provides control only typewriter effect.
/// Typewriter mode is achived by calling <see cref="UVisualTextBlock::SetText"/> method.
/// Use of embedded images in this mode may lead to unexpected behavior, other features that <c>Rich Text Block</c> has are supported.
/// </remarks>
UCLASS(meta = (ToolTip = "Supports Visual Novel style of typewriter effect"))
class VISUALU_API UVisualTextBlock : public URichTextBlock
{
	GENERATED_BODY()
	
public:
	UVisualTextBlock(const FObjectInitializer& ObjectInitializer);

	/// <summary>
	/// Sets text for the <c>Text Block</c> as if it is typed on typewriter or keyboard.
	/// Calling it again while the text visualization is still in progress or with incorrect values of controlling fields
	/// will cause the whole text to show up immediately.
	/// </summary>
	/// <param name="InText">Text to display</param>
	virtual void SetText(const FText& InText) override;

	/// <summary>
	/// Sets the desired amount of characters in one line.
	/// </summary>
	/// <param name="InLineWidth">Amount of characters in one line</param>
	/// \warning <see cref="UVisualTextBlock::LineWidth"/> does not guarantee specified amount of characters to be in one line
	UFUNCTION(BlueprintCallable, Category = "Visual Text Block", meta = (ToolTip = "Sets the desired amount of characters in one line."))
	void SetLineWidth(int InLineWidth);

	/// <summary>
	/// Sets the delay for next character to appear.
	/// </summary>
	/// <param name="Delay">Delay in seconds</param>
	UFUNCTION(BlueprintCallable, Category = "Visual Text Block", meta = (ToolTip = "Sets the delay for next character to appear."))
	void SetCharacterAppearanceDelay(float Delay);

	/// <returns>Delay, in seconds, for the next character to appear</returns>
	UFUNCTION(BlueprintGetter, meta = (ToolTip = "Delay, in seconds, for the next character to appear"))
	FORCEINLINE float GetCharacterAppearanceDelay() const { return CharacterAppearanceDelay; };

	/// <summary>
	/// Whether or not the typewriter effect is active.
	/// </summary>
	/// <returns><c>true</c> if the text is displayed as self-typed</returns>
	UFUNCTION(BlueprintCallable, Category = "Visual Text Block", meta = (ToolTip = "Whether or not the typewriter effect is active."))
	FORCEINLINE bool IsAppearingText() const { return bIsAppearingText; };

	/// <summary>
	/// Determines whether the text block displays the whole text instantly or in self-typed way.
	/// </summary>
	/// <param name="ShouldDisplayInstantly">How text should be displayed</param>
	UFUNCTION(BlueprintCallable, Category = "Visual Text Block", meta = (ToolTip = "Determines whether the text block displays the whole text instantly or in self-typed way"))
	void SetDisplayMode(bool ShouldDisplayInstantly);

	/// <summary>
	/// How text will be visualized during <see cref="UVisualTextBlock::SetText"/> call.
	/// </summary>
	/// <returns><c>true</c> if the text would be displayed instantly</returns>
	UFUNCTION(BlueprintCallable, Category = "Visual Text Block", meta = (ToolTip = "How text will be visualized during SetText call."))
	FORCEINLINE bool GetDisplayMode() const { return bDisplayInstantly; };

	/// <returns>desired amount of characters in one line</returns>
	UFUNCTION(BlueprintCallable, Category = "Visual Text Block", meta = (ToolTip = "desired amount of characters in one line"))
	FORCEINLINE int GetLineWidth() const { return LineWidth; };

	/// <summary>
	/// Pauses on going display of the text. 
	/// </summary>
	/// <returns><c>true</c> if the text was paused</returns>
	UFUNCTION(BlueprintCallable, Category = "Visual Text Block", meta = (ToolTip = "Pauses on going display of the text."))
	bool PauseTextDisplay();

	/// <summary>
	/// Resumes text display.
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Visual Text Block", meta = (ToolTip = "Resumes text display."))
	void UnPauseTextDisplay();

	/// <summary>
	/// Broadcasts the encountered action in the text.
	/// </summary>
	UPROPERTY(BlueprintAssignable, Category = "Visual Text Block", meta = (ToolTip = "Broadcasts the encountered action in the text."))
	FOnActionEncountered OnActionEncountered;

protected:
	/// <summary>
	/// Delay, in seconds, for next character to appear. Must be more than zero to take effect.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, BlueprintGetter = GetCharacterAppearanceDelay, Category = "Visual Text Block", meta = (ToolTip = "Delay, in seconds, for the next character to appear", Units = "s", UIMin = 0.01f, ClampMin = 0.01f))
	float CharacterAppearanceDelay = 0.04f;

	/// <summary>
	/// Desired number of characters for one line. Must be more or equal 2 to take effect.
	/// </summary>
	/// \warning Does not guarantee that each line would have exact specified amount of characters.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual Text Block", meta = (ToolTip = "Desired number of characters for one line. Must be more or equal 2 to take effect. Does not guarantee that each line would have exact specified amount of characters.", Delta = 1.f, UIMin = 2, ClampMin = 2))
	int32 LineWidth;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

private:
	FTimerHandle CharacterDelayTimer;

	FTimerDynamicDelegate CharacterDelayDelegate;

	const UVisualUSettings* VisualUSettings;

	/// \internal
	/// <summary>
	/// Displays one character from provided text. It is specified as a method in order to build a plugin, and it shouldn't be used anywhere else.
	/// </summary>
	UFUNCTION()
	void DisplayOneCharacter();

	/// \internal
	/// <summary>
	/// Parse text for metacharacters and remove them for final display.
	/// </summary>
	UFUNCTION()
	void CheckForActions();

	FString CurrentString;

	FString TextString;

	uint32 TextLength;

	uint32 CurrCharCnt;

	bool bIsAppearingText;

	bool bDisplayInstantly;
};
