// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VisualImage.h"
#include "UObject/ObjectMacros.h"
#include "InfoAssignable.h"
#include "BackgroundVisualImage.generated.h"

class SBackgroundVisualImage;
class UMaterialInstanceDynamic;
class UPaperFlipbook;
class UTexture;

/// <summary>
/// Visual image that supports transition effects through material.
/// </summary>
/// \attention Not blueprintable
UCLASS()
class UBackgroundVisualImage final : public UVisualImage
{
	GENERATED_BODY()

public:
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	UBackgroundVisualImage(const FObjectInitializer& ObjectInitializer);

	/// <summary>
	/// Change state of the Background Visual Image.
	/// </summary>
	/// <remarks>
	/// Called with <c>false</c> when driving animation ends.
	/// </remarks>
	/// <param name="IsTransitioning">New state of the Background Visual Image</param>
	void StopTransition();
	
	/// <returns>Whether or not background is in transiton process</returns>
	bool IsTransitioning() const;

	/// <summary>
	/// Play transition from current Flipbook to Target.
	/// </summary>
	/// <param name="Target">New flipbook that would be displayed by this image after transition ends</param>
	/// <param name="Transition">Material that serves as transition effect</param>
	/// <param name="bShouldAnimateTarget">Whether or not Target flipbook should be animated. If false, will display first frame of the flipbook.</param>
	void PlayTransition(UPaperFlipbook* Target, UMaterialInstanceDynamic* Transition, bool bShouldAnimateTarget);

	void PlayTransition(UPaperFlipbook* Target, UMaterialInstanceDynamic* Transition, int32 FrameIndex);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif
};
