// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SVisualImage.h"

class UMaterialInstanceDynamic;
class UPaperFlipbook;
class UPaperSprite;
class UTexture;

class VISUALU_API SBackgroundVisualImage final : public SVisualImage
{
public:
	SBackgroundVisualImage();

	void SetTransition(UPaperFlipbook* TargetFlipbook, UMaterialInstanceDynamic* TransitionMaterial, bool bShouldAnimateTarget);

	void SetTransitionState(bool IsTransitioning);

	FORCEINLINE bool IsTransitioning() const { return bIsTransitioning; }

private:
	virtual UObject* GetFinalResource() const override;

	UPaperSprite* GetTargetSprite() const;

private:
	UMaterialInstanceDynamic* Transition;

	UPaperFlipbook* Target;

	bool bIsTransitioning;

	bool bIsTargetAnimated;
};
