// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Scenario.h"
#include "Engine/DataTable.h"
#include "VisualController.h"
#include "VisualTemplates.h"
#include "VisualVersioningSubsystem.generated.h"

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
	UVisualVersioningSubsystem();

	/*
	* Chooses a provided version of Data Table in Visual Controller.
	* @param Version new scenario information.
	* Returns result of choosing data table.
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Versioning")
	bool ChooseVersion(UVisualController* VisualController, const UDataTable* DataTable, const FName& SceneName, const FVisualScenarioInfo& Version);

	template<typename T = FVisualScenarioInfo, typename... V>
	inline bool ChooseVersion(UVisualController* VisualController, const UDataTable* DataTable, const FName& SceneName, V T::*... Members, const V&... Values)
	{
		check(VisualController);

		FScenario* Scene = GetSceneChecked(DataTable, SceneName);
		Versions.Add(Scene, Scene->Info);
		UpdateMembers(Scene->Info, Members, Values);

		return VisualController->RequestNode(DataTable);
	}

	template<typename T = FVisualScenarioInfo, typename... V>
	inline bool ChooseVersion(UVisualController* VisualController, FScenario* Scene, V T::*... Members, const V&... Values)
	{
		check(VisualController);
		check(Scene);

		Versions.Add(Scene, Scene->Info);
		UpdateMembers(Scene->Info, Members, Values);

		return VisualController->RequestNode(Scene->Owner);
	}

	/*
	* Switches scene to an older version.
	*/
	void Checkout(FScenario* const Scene) const;
	
	virtual void Deinitialize() override;

private:
	/*
	* Returns scenario from data table at given location.
	* Will trigger assertion for invalid data table and index.
	*/
	FScenario* GetSceneChecked(const UDataTable* DataTable, const FName& SceneName) const;

private:
	TMultiMap<FScenario*, FVisualScenarioInfo> Versions;

};
