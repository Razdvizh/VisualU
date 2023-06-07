// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VisualSprite.generated.h"

class UPaperFlipbook;

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
	virtual void AssignExpressions(const TArray<TSoftObjectPtr<UPaperFlipbook>>& InExpressions);
};
