// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "AsyncLoadable.h"
#include "InfoAssignable.h"
#include "VisualImageBase.generated.h"

UCLASS(abstract)
class VISUALU_API UVisualImageBase : public UWidget, public IAsyncLoadable, public IInfoAssignable
{
	GENERATED_BODY()

protected:
	UVisualImageBase(const FObjectInitializer& ObjectInitializer);
};
