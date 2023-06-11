// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlock.h"
#include "VisualTextBlock.generated.h"

class UVisualUSettings;

/**
 * Supports Visual Novel style of text wrapping
 */
UCLASS()
class VISUALU_API UVisualTextBlock : public URichTextBlock
{
	GENERATED_BODY()
	
public:
	UVisualTextBlock(const FObjectInitializer& ObjectInitializer);

	virtual void SetText(const FText& InText) override;

	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	void SetLineWidth(int InLineWidth);

	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	void SetCharacterAppearanceDelay(float Delay);

	UFUNCTION(BlueprintGetter)
	FORCEINLINE float GetCharacterAppearanceDelay() const { return CharacterAppearanceDelay; };

	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	FORCEINLINE bool IsAppearingText() const { return bIsAppearingText; };

	UFUNCTION(BlueprintCallable, Category = "Visual Text Block", meta = (ToolTip = "Determines whether the text block displays the whole text instantly or in by-character manner"))
	void SetDisplayMode(bool ShouldDisplayInstantly);

	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	FORCEINLINE bool GetDisplayMode() const { return bDisplayInstantly; };

	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	FORCEINLINE int GetLineWidth() const { return LineWidth; };

	UFUNCTION(BlueprintCallable, Category = "Visual Text Block", meta = (ToolTip = "Returns the last encountered action during text display"))
	FORCEINLINE EVisualTextAction GetCurrentAction() const { return CurrentAction; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, BlueprintGetter = GetCharacterAppearanceDelay, Category = "Visual Text Block", meta = (Units = "s"))
	float CharacterAppearanceDelay = 0.04f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual Text Block", meta = (ToolTip = "Desired number of characters for one line. Must be more or equal 2 to take effect. Does not guarantee that each line would have this amount of characters.", Delta = 1.f, UIMin = 2, ClampMin = 2))
	int LineWidth;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

private:
	FTimerHandle CharacterDelayTimer;

	FTimerDynamicDelegate CharacterDelayDelegate;

	const UVisualUSettings* VisualUSettings;

	UFUNCTION()
	void DisplayOneCharacter();

	void SetCurrentAction(const EVisualTextAction& Action);

	EVisualTextAction CurrentAction;

	FString CurrentString;

	FString TextString;

	uint32 TextLength;

	uint32 CurrCharCnt;

	bool bIsAppearingText;

	bool bDisplayInstantly;
};
