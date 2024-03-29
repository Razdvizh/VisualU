// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "AsyncLoadable.h"
#include "InfoAssignable.h"
#include "VisualImageBase.generated.h"

/// <summary>
/// Base class for custom UMG widgets that serve as FSlateBrush containers.
/// </summary>
/// <remarks>
/// Supports asynchronous loading, <see cref="FVisualInfo">Visual Info</see> assignment through interfaces.
/// </remarks>
UCLASS(abstract)
class VISUALU_API UVisualImageBase : public UWidget, public IAsyncLoadable, public IInfoAssignable
{
	GENERATED_BODY()

protected:
	UVisualImageBase(const FObjectInitializer& ObjectInitializer);
};
