// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "VisualVersioningSubsystem.generated.h"

class UDataTable;
class UVisualController;
struct FVisualScenarioInfo;

/**
 * Allows for altering scenarios in chosen Data Tables.
 */
UCLASS()
class VISUALU_API UVisualVersioningSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	UVisualVersioningSubsystem();

	/*
	* Chooses a provided version of Data Table in Visual Controller
	* Returns result of choosing
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Versioning")
	bool ChooseVersion(UVisualController* VisualController, const UDataTable* DataTable, int32 Index, const FVisualScenarioInfo& Version);

	/*
	* Switches subsystem to an older version
	*/
	void Checkout(FScenario* const Scene) const;

	virtual void Deinitialize() override;
	
private:
	TMultiMap<FScenario*, FVisualScenarioInfo> Versions;

};
