// Copyright (c) 2024 Evgeny Shustov

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "InfoAssignable.h"
#include "VisualImageBase.generated.h"

/**
* Base class for UMG widgets that display slate brushes.
* 
* @see IInfoAssignable
*/
UCLASS(abstract)
class VISUALU_API UVisualImageBase : public UWidget, public IInfoAssignable
{
	GENERATED_BODY()

protected:
	/**
	* Abstract, cannot be instantiated.
	*/
	UVisualImageBase(const FObjectInitializer& ObjectInitializer);

};
