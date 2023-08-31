// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class UMaterialInstanceDynamic;
class UPaperFlipbook; 

class VISUALU_API FTransitionMaterialProxy
{
public:
static UMaterialInstanceDynamic* GetTransitionMaterial(UMaterialInstanceDynamic* MaterialToChange, const TMap<FName, UTexture*>& Params);

};
