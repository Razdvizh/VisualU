// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SVisualImage.h"

class UMaterialInstanceDynamic;
class UPaperFlipbook;
class UPaperSprite;
class UTexture;

/// <summary>
/// Slate visual image that supports transition effects through material.
/// </summary>
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
	
	void Construct(const FArguments& Args);

	void StartTransition(UPaperFlipbook* TargetFlipbook, UMaterialInstanceDynamic* TransitionMaterial, bool bShouldAnimateTarget);

	void StartTransition(UPaperFlipbook* TargetFlipbook, UMaterialInstanceDynamic* TransitionMaterial, int32 FrameIndex);

	void StopTransition();

	FORCEINLINE bool IsTransitioning() const { return bIsTransitioning; }

protected:
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	virtual FString GetReferencerName() const override;

private:
	virtual UObject* GetFinalResource() const override;

	UPaperSprite* GetTargetSprite() const;

private:
	TSlateAttribute<UMaterialInstanceDynamic*> Transition;

	TSlateAttribute<UPaperFlipbook*> Target;

	bool bIsTransitioning;

	bool bIsTargetAnimated;

	int32 TargetFrameIndex;
};
