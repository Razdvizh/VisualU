// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VisualChoiceSubsystem.generated.h"

class UDataTable;
struct FScenarioVisualInfo;

/// \todo add async loading for data table assets
UCLASS()
class VISUALU_API UVisualChoiceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UVisualChoiceSubsystem();

	UFUNCTION(BlueprintCallable, Category = "Visual Choice Subsystem")
	void ChooseScenarioVariantByName(UDataTable* Node, FName ScenarioName, const FScenarioVisualInfo& ScenarioVariant);

};
