// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VisualDashboard.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UVisualDashboard : public UInterface
{
	GENERATED_BODY()
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTextDisplayFinished, EVisualControllerDirection::Type, Direction);

/**
 * Interface for dashboard - a control panel common for visual novels.
 */
class VISUALU_API IVisualDashboard
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Visual Dashboard")
	FOnTextDisplayFinished GetTextDisplayFinishedDelegate() const;

};
