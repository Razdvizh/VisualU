// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualVersioningSubsystem.h"
#include "VisualUCustomVersion.h"

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

void UVisualVersioningSubsystem::Experimental_SerializeSubsystem(FArchive& Ar)
{
	Ar.UsingCustomVersion(FVisualUCustomVersion::GUID);

	if (Ar.IsSaving())
	{
		TSet<FScenario*> Scenes;
		Versions.GetKeys(Scenes);
		int32 NumScenes = Scenes.Num();
		Ar << NumScenes;
		for (FScenario*& Scene : Scenes)
		{
			Ar << *Scene;
			TArray<FVisualScenarioInfo> Infos;
			Versions.MultiFind(Scene, Infos, /*bMaintainOrder=*/true);
			Ar << Infos;
		}
	}
	else
	{
		int32 NumScenes;
		Ar << NumScenes;
		Versions.Reserve(NumScenes);
		for (int32 i = 0; i < NumScenes; i++)
		{
			FScenario Scene;
			Ar << Scene;
			TArray<FVisualScenarioInfo> Infos;
			Ar << Infos;
			for (FVisualScenarioInfo& Info : Infos)
			{
				Versions.Add(FScenario::ResolveScene(Scene), Info);
			}
		}
	}
}

void UVisualVersioningSubsystem::Deinitialize()
{
	TSet<FScenario*> Scenes;
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
