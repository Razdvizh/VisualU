// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "VisualVersioningSubsystem.generated.h"

struct FScenario;
class UDataTable;
class UVisualController;

/**
 * Allows for altering scenarios in chosen Data Tables.
 */
UCLASS()
class VISUALU_API UVisualVersioningSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Deinitialize() override;

private:
	TSet<FScenario*> InitialScenes;

};
