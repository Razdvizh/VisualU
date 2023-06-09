// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VisualImage.h"
#include "VisualSprite.generated.h"

/**
 * 
 */
UCLASS(abstract)
class VISUALU_API UVisualSprite : public UUserWidget
{
	GENERATED_BODY()
		
public:
	UVisualSprite(const FObjectInitializer& ObjectInitializer);

	/*TODO: Make it implementable in Blueprints; think of more generic version*/
	UFUNCTION(BlueprintCallable, Category = "Visual Sprite")
	virtual void AssignVisualImageInfo(const TArray<FVisualImageInfo>& InInfo);
};
