// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualVersioningSubsystem.h"
#include "Engine/DataTable.h"
#include "VisualController.h"
#include "Scenario.h"

UVisualVersioningSubsystem::UVisualVersioningSubsystem()
	: Super(),
	Versions()
{
}

bool UVisualVersioningSubsystem::ChooseVersion(UVisualController* VisualController, const UDataTable* DataTable, int32 Index, const FVisualScenarioInfo& Version)
{
	check(VisualController);
	check(DataTable);

	TArray<FScenario*> Rows;
	DataTable->GetAllRows(UE_SOURCE_LOCATION, Rows);
	checkf(Rows.IsValidIndex(Index), TEXT("Can't find scene at position: %i in Data Table: %s"), Index, *DataTable->GetFName().ToString());

	FScenario* Scene = Rows[Index];
	Versions.Add(Scene, Scene->Info);
	Scene->Info = Version;

	return VisualController->RequestNode(DataTable);
}

void UVisualVersioningSubsystem::Checkout(FScenario* const Scene) const
{
	check(Scene);
	const FVisualScenarioInfo& Version = Versions.FindChecked(Scene);
	Scene->Info = Version;
}

void UVisualVersioningSubsystem::Deinitialize()
{
	TArray<FScenario*> Scenes;
	Versions.GetKeys(Scenes);
	for (FScenario*& Scene : Scenes)
	{
		TArray<FVisualScenarioInfo> Infos;
		Versions.MultiFind(Scene, Infos, /*bMaintainOrder=*/true);
		if (!Infos.IsEmpty())
		{
			//Reset scene to initial, asset state
			Scene->Info = Infos[0];
		}
	}
}
