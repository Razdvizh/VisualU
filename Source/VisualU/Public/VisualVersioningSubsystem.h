// Copyright (c) 2024 Evgeny Shustov

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
 * Allows for altering scenes in chosen nodes.
 * All versions are applied and discared at runtime.
 */
UCLASS()
class VISUALU_API UVisualVersioningSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	UVisualVersioningSubsystem();

	/**
	* Chooses data table with scene altered by provided version.
	* 
	* @param VisualController controller that will request node
	* @param DataTable data table that contains desired scene
	* @param SceneName row name of the desired scene
	* @param Version altered scene
	* @return result of requesting node
	* 
	* @see UVisualUBlueprintStatics::Choose()
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Versioning")
	bool ChooseVersion(UVisualController* VisualController, const UDataTable* DataTable, const FName& SceneName, const FVisualScenarioInfo& Version);

	/**
	* Template version.
	* 
	* @see UVisualVersioningSubsystem::ChooseVersion(UVisualController*, const UDataTable*, const FName&, const FVisualScenarioInfo&)
	*/
	template<typename T = FVisualScenarioInfo, typename... V>
	inline bool ChooseVersion(UVisualController* VisualController, const UDataTable* DataTable, const FName& SceneName, V T::*... Members, const V&... Values)
	{
		check(VisualController);

		FScenario* Scene = GetSceneChecked(DataTable, SceneName);
		Versions.Add(Scene, Scene->Info);
		UpdateMembers(Scene->Info, Members, Values);

		return VisualController->RequestNode(DataTable);
	}

	/**
	* Template version.
	* 
	* @see UVisualVersioningSubsystem::ChooseVersion(UVisualController*, const UDataTable*, const FName&, const FVisualScenarioInfo&)
	*/
	template<typename T = FVisualScenarioInfo, typename... V>
	inline bool ChooseVersion(UVisualController* VisualController, FScenario* Scene, V T::*... Members, const V&... Values)
	{
		check(VisualController);
		check(Scene);

		Versions.Add(Scene, Scene->Info);
		UpdateMembers(Scene->Info, Members, Values);

		return VisualController->RequestNode(Scene->Owner);
	}

	/**
	* Switches scene to an older version.
	* Has no effect for scene not altered by this subsystem.
	* 
	* @param Scene scene to revert back to the previous version
	*/
	void Checkout(FScenario* const Scene) const;

	/**
	* Not ready for production code.
	*
	* @param Ar archive to serialize this subsystem
	*/
	virtual void SerializeSubsystem_Experimental(FArchive& Ar);
	
	/**
	* Discards all versions bringing scenes to original state.
	* Called when local player is destroyed.
	*/
	virtual void Deinitialize() override;

private:
	/**
	* Will trigger assertion for invalid data table or name.
	* 
	* @param DataTable data table that contains desired scene
	* @param SceneName row name of desired scene
	* @return scene from data table at given location
	*/
	FScenario* GetSceneChecked(const UDataTable* DataTable, const FName& SceneName) const;

private:
	/**
	* Map of scenes and their information before being altered.
	*/
	TMultiMap<FScenario*, FVisualScenarioInfo> Versions;

};
