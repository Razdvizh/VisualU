// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "VisualVersioningSubsystem.generated.h"

class UDataTable;
class UVisualController;
struct FScenarioVisualInfo;

/**
 * Allows for altering scenarios in chosen Data Tables.
 */
UCLASS()
class VISUALU_API UVisualVersioningSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Visual Versioning", meta = (DeprecatedFunction))
	void ChooseVersion(UVisualController* VisualController, const UDataTable* DataTable, int32 Index, const FScenarioVisualInfo& Version);

	virtual void Deinitialize() override;
	
};
