// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualVersioningSubsystem.h"
#include "Engine/DataTable.h"
#include "VisualController.h"
#include "Scenario.h"

void UVisualVersioningSubsystem::ChooseVersion(UVisualController* VisualController, const UDataTable* DataTable, int32 Index, const FScenarioVisualInfo& Version)
{
	check(VisualController);
	check(DataTable);

	TArray<FScenario*> Rows;
	DataTable->GetAllRows(UE_SOURCE_LOCATION, Rows);
	InitialScenes.Add(Rows[Index]);
	Rows[Index]->AssignScenarioVisualInfo(Version);
}

void UVisualVersioningSubsystem::Deinitialize()
{
	
}
