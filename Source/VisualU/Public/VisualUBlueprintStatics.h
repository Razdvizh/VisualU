// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VisualUBlueprintStatics.generated.h"

class UPaperSprite;

///<summary>
/// Provides useful functionality that are missing in the Blueprints
///</summary>
UCLASS()
class VISUALU_API UVisualUBlueprintStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/// <summary>
	/// Get the texture of <paramref name="Sprite"/> that should be rendered.
	/// </summary>
	/// <param name="Sprite">Sprite to get texture from</param>
	UFUNCTION(BlueprintCallable, Category = "VisualU|Paper Sprite", meta = (ToolTip = "Get the texture of Sprite that should be rendered"))
	static UTexture2D* GetSpriteTexture(UPaperSprite* Sprite);
};
