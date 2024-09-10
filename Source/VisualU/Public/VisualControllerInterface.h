// Copyright (c) 2024 Evgeny Shustov

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VisualControllerInterface.generated.h"

class UVisualController;

UINTERFACE(MinimalAPI)
class UVisualControllerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Indicates that the class has access to UVisualController.
 * 
 * @see UVisualController
 */
class VISUALU_API IVisualControllerInterface
{
	GENERATED_BODY()

public:
	/**
	* @return UVisualController from the class that implements the interface.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Visual Controller Interface")
	UVisualController* GetVisualController() const;

};
