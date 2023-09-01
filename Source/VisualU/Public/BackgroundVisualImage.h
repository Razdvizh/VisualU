// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VisualImage.h"
#include "UObject/ObjectMacros.h"
#include "BackgroundVisualImage.generated.h"

class SBackgroundVisualImage;
class UMaterialInstanceDynamic;
class UPaperFlipbook;
class UTexture;

USTRUCT(BlueprintType)
struct FVisualBackgroundImageInfo : public FVisualImageInfo
{
	GENERATED_USTRUCT_BODY()

	//\todo add fields
};

UCLASS()
class UBackgroundVisualImage final : public UVisualImage
{
	GENERATED_BODY()

public:
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	UBackgroundVisualImage(const FObjectInitializer& ObjectInitializer);

	void SetTransitionState(bool IsTransitioning);
	
	bool IsTransitioning() const;

	void PlayTransition(UPaperFlipbook* Target, UMaterialInstanceDynamic* Transition, bool bShouldAnimateTarget);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif
};
