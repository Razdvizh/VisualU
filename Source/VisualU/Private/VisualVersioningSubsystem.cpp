// Copyright (c) 2024 Evgeny Shustov


#include "VisualVersioningSubsystem.h"
#include "VisualUCustomVersion.h"

UVisualVersioningSubsystem::UVisualVersioningSubsystem()
	: Super(),
	Versions()
{
}

void UVisualVersioningSubsystem::AlterDataTable(const UDataTable* DataTable, const FName& SceneName, const FVisualScenarioInfo& Version)
{
	FScenario* Scene = GetSceneChecked(DataTable, SceneName);
	FScenarioId Id{Scene->GetOwner(), Scene->GetIndex()};
	Versions.Add(MoveTemp(Id), Scene->Info);
	Scene->Info = Version;
}

void UVisualVersioningSubsystem::Checkout(FScenario* Scene) const
{
	check(Scene);
	FScenarioId Id{ Scene->GetOwner(), Scene->GetIndex() };
	if (const FVisualScenarioInfo* Version = Versions.Find(Id))
	{
		Scene->Info = *Version;
	}
}

void UVisualVersioningSubsystem::CheckoutAll(const UDataTable* DataTable) const
{
	check(DataTable);
	TArray<FScenario*> Rows;
	DataTable->GetAllRows(UE_SOURCE_LOCATION, Rows);

	for (FScenario*& Row : Rows)
	{
		Checkout(Row);
	}
}

void UVisualVersioningSubsystem::SerializeSubsystem(FArchive& Ar)
{
	Ar.UsingCustomVersion(FVisualUCustomVersion::GUID);

	if (Ar.IsSaving())
	{
		TSet<FScenarioId> SceneIDs;
		Versions.GetKeys(SceneIDs);
		int32 NumScenes = SceneIDs.Num();
		Ar << NumScenes;
		for (FScenarioId& SceneId : SceneIDs)
		{
			FScenario* Scene = GetSceneChecked(SceneId);
			Ar << *Scene;
			TArray<FVisualScenarioInfo> Infos;
			Versions.MultiFind(SceneId, Infos, /*bMaintainOrder=*/true);
			Infos.Add(Scene->Info);
			Ar << Infos;
		}
	}
	else
	{
		int32 NumScenes = 0;
		Ar << NumScenes;
		Versions.Reserve(NumScenes);
		for (int32 i = 0; i < NumScenes; i++)
		{
			FScenario Scene;
			Ar << Scene;
			TArray<FVisualScenarioInfo> Infos;
			Ar << Infos;
			FScenario* ResolvedScene = FScenario::ResolveScene(Scene);
			ResolvedScene->Info = Infos.Pop();
			for (FVisualScenarioInfo& Info : Infos)
			{
				FScenarioId Id {Scene.GetOwner(), Scene.GetIndex()};
				Versions.Add(Id, Info);
			}
		}
	}
}

void UVisualVersioningSubsystem::Deinitialize()
{
	TSet<FScenarioId> SceneIDs;
	Versions.GetKeys(SceneIDs);
	for (FScenarioId& SceneId : SceneIDs)
	{
		FScenario* Scene = GetSceneChecked(SceneId);
		TArray<FVisualScenarioInfo> Infos;
		Versions.MultiFind(SceneId, Infos, /*bMaintainOrder=*/true);
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

FScenario* UVisualVersioningSubsystem::GetSceneChecked(const FScenarioId& Id) const
{
	checkf(!Id.SoftOwner.IsNull(), TEXT("Can't identify a scene with invalid owner."));

	const UDataTable* Owner = Id.SoftOwner.LoadSynchronous();
	check(Owner);

	TArray<FScenario*> Rows;
	Owner->GetAllRows(UE_SOURCE_LOCATION, Rows);

	check(Rows.IsValidIndex(Id.Index));

	return Rows[Id.Index];
}
