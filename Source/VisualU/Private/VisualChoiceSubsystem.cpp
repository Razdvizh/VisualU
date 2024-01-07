// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualChoiceSubsystem.h"
#include "Engine/DataTable.h"
#include "Scenario.h"

UVisualChoiceSubsystem::UVisualChoiceSubsystem() : Super()
{
}

void UVisualChoiceSubsystem::ChooseScenarioVariantByName(UDataTable* Node, FName ScenarioName, const FScenarioVisualInfo& ScenarioVariant)
{
	if (FScenario* Scenario = Node->FindRow<FScenario>(ScenarioName, UE_SOURCE_LOCATION))
	{
		Scenario->AssignScenarioVisualInfo(ScenarioVariant);
	}
}
