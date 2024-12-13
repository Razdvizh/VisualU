// Copyright (c) 2024 Evgeny Shustov

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

/**
* Visual image that supports transition effects through material.
*/
UCLASS()
class UBackgroundVisualImage final : public UVisualImage
{
	GENERATED_BODY()

public:
	UBackgroundVisualImage(const FObjectInitializer& ObjectInitializer);

	/**
	* Releases memory allocated for slate widgets.
	* 
	* @param bReleaseChildren should memory of child widgets be released
	*/
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	/**
	* Stops ongoing transition.
	* Has no effect when called during stopped transition.
	*/
	void StopTransition();
	
	/**
	* @return {@code false} when no transition is active.
	*/
	bool IsTransitioning() const;

	/**
	* Plays transition from UVisualImage::Flipbook to UBackgroundVisualImage::Target.
	* 
	* @param Target New flipbook that would be displayed by this image after transition ends
	* @param Transition Material that serves as transition effect
	* @param bShouldAnimateTarget Whether or not UBackgroundVisualImage::Target
	*		 flipbook should be animated. 
	*		 For not animated flipbooks, will display first frame
	*		 of the flipbook.
	*/
	void PlayTransition(UPaperFlipbook* Target, UMaterialInstanceDynamic* Transition, bool bShouldAnimateTarget);

	/**
	* Version for non-animated flipbooks.
	* 
	* @see UBackgroundVisualImage::PlayTransition(UPaperFlipbook*, UMaterialInstanceDynamic*, bool)
	* 
	* @param Target New flipbook that would be displayed by this image after transition ends
	* @param Transition Material that serves as transition effect
	* @param FrameIndex which frame to show after transition ends.
	*/
	void PlayTransition(UPaperFlipbook* Target, UMaterialInstanceDynamic* Transition, int32 FrameIndex);

protected:
	/**
	* @return underlying slate widget
	*/
	virtual TSharedRef<SWidget> RebuildWidget() override;

};
