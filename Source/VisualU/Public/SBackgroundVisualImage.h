// Copyright (c) 2024 Evgeny Shustov

#pragma once

#include "CoreMinimal.h"
#include "SVisualImage.h"

class UMaterialInstanceDynamic;
class UPaperFlipbook;
class UPaperSprite;
class UTexture;

/**
* Slate visual image that supports transition effects through material.
*/
class VISUALU_API SBackgroundVisualImage final : public SVisualImage
{

	SLATE_DECLARE_WIDGET(SBackgroundVisualImage, SVisualImage)

	SLATE_BEGIN_ARGS(SBackgroundVisualImage) : _IsTransitioning(false), _IsTargetAnimated(false), _TargetFrameIndex(0) {}

		SLATE_ARGUMENT(bool, IsTransitioning)

		SLATE_ARGUMENT(bool, IsTargetAnimated)

		SLATE_ARGUMENT(int32, TargetFrameIndex)

		SLATE_ATTRIBUTE(UMaterialInstanceDynamic*, Transition)

		SLATE_ATTRIBUTE(UPaperFlipbook*, Target)

	SLATE_END_ARGS()

public:
	SBackgroundVisualImage();
	
	/**
	* Constructor call for slate declarative syntax.
	* 
	* @param Args slate arguments
	*/
	void Construct(const FArguments& Args);

	/**
	* @see UBackgroundVisualImage::StartTransition(UPaperFlipbook*, UMaterialInstanceDynamic*, bool)
	*/
	void StartTransition(UPaperFlipbook* TargetFlipbook, UMaterialInstanceDynamic* TransitionMaterial, bool bShouldAnimateTarget);

	/**
	* @see UBackgroundVisualImage::StartTransition(UPaperFlipbook*, UMaterialInstanceDynamic*, int32)
	*/
	void StartTransition(UPaperFlipbook* TargetFlipbook, UMaterialInstanceDynamic* TransitionMaterial, int32 FrameIndex);

	/**
	* @see UBackgroundVisualImage::StopTransition()
	*/
	void StopTransition();

	/**
	* @see UBackgroundVisualImage::IsTransitioning()
	*/
	FORCEINLINE bool IsTransitioning() const { return bIsTransitioning; }

protected:
	/**
	* @see SVisualImage::AddReferencedObjects()
	*/
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	/**
	* @see SVisualImage::GetReferencerName()
	*/
	virtual FString GetReferencerName() const override;

private:
	/**
	* @see SVisualImageBase::GetFinalResource()
	*/
	virtual UObject* GetFinalResource() const override;

	/**
	* Retrieves sprite from SBackgroundVisualImage::Target.
	*/
	UPaperSprite* GetTargetSprite() const;

private:
	/**
	* Material instance to be rendered as transition.
	*/
	TSlateAttribute<UMaterialInstanceDynamic*> Transition;

	/**
	* Sprite that will be rendered after transition ends.
	*/
	TSlateAttribute<UPaperFlipbook*> Target;

	/**
	* @see UBackgroundVisualImage::IsTransitioning()
	*/
	bool bIsTransitioning;

	/**
	* @see UBackgroundVisualImage::StartTransition(UPaperFlipbook*, UMaterialInstanceDynamic*, bool)
	*/
	bool bIsTargetAnimated;

	/**
	* @see UBackgroundVisualImage::StartTransition(UPaperFlipbook*, UMaterialInstanceDynamic*, int32)
	*/
	int32 TargetFrameIndex;
};
