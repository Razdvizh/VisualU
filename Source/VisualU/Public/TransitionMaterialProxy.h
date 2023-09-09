// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class UMaterialInstanceDynamic;
class UPaperFlipbook; 

/// <summary>
/// Proxy object for dynamic material designated for transition effect rendering.
/// </summary>
class VISUALU_API FTransitionMaterialProxy
{
public:
/// <summary>
/// Applies given parameters to the provided dynamic material.
/// </summary>
/// <param name="MaterialToChange">Material that should render transition</param>
/// <param name="Params">Parameters to apply</param>
/// <returns>Altered dynamic material</returns>
static UMaterialInstanceDynamic* GetTransitionMaterial(UMaterialInstanceDynamic* MaterialToChange, const TMap<FName, UTexture*>& Params);

};
