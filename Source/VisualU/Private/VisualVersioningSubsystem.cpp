// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualVersioningSubsystem.h"

UVisualVersioningSubsystem::UVisualVersioningSubsystem()
	: Super(),
	Versions()
{
}

bool UVisualVersioningSubsystem::ChooseVersion(UVisualController* VisualController, const UDataTable* DataTable, const FName& SceneName, const FVisualScenarioInfo& Version)
{
	check(VisualController);

	FScenario* Scene = GetSceneChecked(DataTable, SceneName);
	Versions.Add(Scene, Scene->Info);
	Scene->Info = Version;
	
	return VisualController->RequestNode(DataTable);
}

void UVisualVersioningSubsystem::Checkout(FScenario* const Scene) const
{
	check(Scene);
	if (const FVisualScenarioInfo* Version = Versions.Find(Scene))
	{
		Scene->Info = *Version;
	}
}

void UVisualVersioningSubsystem::Deinitialize()
{
	TArray<FScenario*> Scenes;
	Versions.GetKeys(Scenes);
	for (FScenario*& Scene : Scenes)
	{
		check(Scene);
		TArray<FVisualScenarioInfo> Infos;
		Versions.MultiFind(Scene, Infos, /*bMaintainOrder=*/true);
		if (!Infos.IsEmpty())
		{
			//Reset scene to initial, asset state
			Scene->Info = Infos[0];
		}
	}
}

FScenario* UVisualVersioningSubsystem::GetSceneChecked(const UDataTable* DataTable, const FName& SceneName) const
{
	check(DataTable);
	
	FScenario* Scene = DataTable->FindRow<FScenario>(SceneName, UE_SOURCE_LOCATION, /*bWarnIfMissing=*/false);
	checkf(Scene, TEXT("Can't find scene with name: %s in Data Table: %s"), *SceneName.ToString(), *DataTable->GetFName().ToString());

	return Scene;
}
