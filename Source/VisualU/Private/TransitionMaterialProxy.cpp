// Fill out your copyright notice in the Description page of Project Settings.


#include "TransitionMaterialProxy.h"
#include "Materials/MaterialInstanceDynamic.h"

UMaterialInstanceDynamic* FTransitionMaterialProxy::GetTransitionMaterial(UMaterialInstanceDynamic* MaterialToChange, const TMap<FName, UTexture*>& Params)
{
    for (auto It = Params.CreateConstIterator(); It; ++It)
    {
        MaterialToChange->SetTextureParameterValue(It.Key(), It.Value());
    }

    return MaterialToChange;
}
