// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VisualControllerInterface.generated.h"

class UVisualController;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UVisualControllerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Indicates that the class has access to Visual Controller.
 */
class VISUALU_API IVisualControllerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	UVisualController* GetVisualController() const;

};
