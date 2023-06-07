// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlock.h"
#include "VisualTextBlock.generated.h"

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
	float GetCharacterAppearanceDelay() const;

	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	bool IsAppearingText() const;

	UFUNCTION(BlueprintCallable, Category = "Visual Text Block", meta = (ToolTip = "Determines whether the text block displays the whole text instantly or in by-character manner"))
	void SetDisplayMode(bool ShouldDisplayInstantly);

	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	bool GetDisplayMode() const;

	UFUNCTION(BlueprintCallable, Category = "Visual Text Block")
	int GetLineWidth() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, BlueprintGetter = GetCharacterAppearanceDelay, Category = "Visual Text Block", meta = (Units = "s"))
	float CharacterAppearanceDelay = 0.04f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual Text Block", meta = (ToolTip = "Desired number of characters for one line. Must be more or equal 2 to take effect. Does not guarantee that each line would have this amount of characters.", Delta = 1.f, UIMin = 2, ClampMin = 2))
	int LineWidth;

	virtual void SynchronizeProperties() override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

private:
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Visual Text Block", meta = (ToolTip = "Font size in pixels where X is width and Y is height. This is used only for debugging", AllowPrivateAccess = true))
	FVector2D FontSize;

private:
	FTimerHandle CharacterDelayTimer;

	FTimerDynamicDelegate CharacterDelayDelegate;

	UFUNCTION()
	void DisplayOneCharacter();

	FString CurrentString;

	FString TextString;

	uint32 TextLength;

	uint32 CurrCharCnt;

	bool bIsAppearingText;

	bool bDisplayInstantly;
};
