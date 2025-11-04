// Copyright (c) 2024 Evgeny Shustov

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Scenario.h"
#include "InfoAssignable.h"
#include "Engine/DataTable.h"
#include "VisualController.h"
#include "VisualTemplates.h"
#include "VisualVersioningSubsystem.generated.h"

class UDataTable;
class UVisualController;

/**
 * Allows for altering scenes in nodes.
 * All versions are applied and discared at runtime.
 */
UCLASS()
class VISUALU_API UVisualVersioningSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

private:
	struct FScenarioId
	{
		TSoftObjectPtr<const UDataTable> SoftOwner;
		int32 Index;

		friend uint32 GetTypeHash(const FScenarioId& Id)
		{
			return HashCombine(GetTypeHash(Id.SoftOwner), Id.Index);
		}

		friend bool operator==(const FScenarioId& Id, const FScenarioId& Other)
		{
			return Id.SoftOwner == Other.SoftOwner &&
				Id.Index == Other.Index;
		}

		friend bool operator!=(const FScenarioId& Id, const FScenarioId& Other)
		{
			return !(Id == Other);
		}
	};
	
public:
	UVisualVersioningSubsystem();

	/**
	* Alters scene in the data table by provided version.
	* 
	* @param DataTable data table that contains desired scene
	* @param SceneName row name of the desired scene
	* @param Version altered scene
	* @return result of requesting node
	* 
	* @see UVisualUBlueprintStatics::Choose()
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual Versioning")
	void AlterDataTable(const UDataTable* DataTable, const FName& SceneName, const FVisualScenarioInfo& Version);

	/**
	* Template version.
	* 
	* @see UVisualVersioningSubsystem::AlterDataTable(const UDataTable*, const FName&, const FVisualScenarioInfo&)
	*/
	template<typename T = FVisualScenarioInfo, typename... V>
	inline void AlterDataTable(const UDataTable* DataTable, const FName& SceneName, V T::*... Members, const V&... Values)
	{
		FScenario* Scene = GetSceneChecked(DataTable, SceneName);
		FScenarioId Id{ Scene->GetOwner(), Scene->GetIndex() };
		Versions.Add(Id, Scene->Info);
		UpdateMembers<T, V...>(&Scene->Info, Members..., Values...);
	}

	/**
	* Template version.
	* 
	* @see UVisualVersioningSubsystem::AlterDataTable(const UDataTable*, const FName&, const FVisualScenarioInfo&)
	*/
	template<typename T = FVisualScenarioInfo, typename... V>
	inline void AlterDataTable(FScenario* Scene, V T::*... Members, const V&... Values)
	{
		check(Scene);
		FScenarioId Id{ Scene->GetOwner(), Scene->GetIndex() };
		Versions.Add(Id, Scene->Info);
		UpdateMembers<T, V...>(&Scene->Info, Members..., Values...);
	}

	/**
	* Switches scene to an older version.
	* Has no effect for scene not altered by this subsystem.
	* 
	* @param Scene scene to revert back to the previous version
	*/
	void Checkout(FScenario* Scene) const;

	/**
	* Switches all scenes in the data table to an older version.
	* Has no effect for scenes not altered by this subsystem.
	*
	* @param DataTable node which scenes will be reverted to previous version
	*/
	void CheckoutAll(const UDataTable* DataTable) const;

	/**
	* Serializes versioning subsystem to the provided archive.
	* Uses FVisualUCustomVersion.
	*
	* @param Ar archive to serialize this subsystem
	*/
	virtual void SerializeSubsystem(FArchive& Ar);

	/**
	* Serializes versioning subsystem to the provided archive.
	* Uses FVisualUCustomVersion.
	*
	* @param Ar archive to serialize this subsystem
	*/
	virtual void Serialize(FArchive& Ar) override;
	
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

	/**
	* Will trigger assertion for invalid scenario id.
	*
	* @param Id identity of requested scene
	* @return identified scene
	*/
	FScenario* GetSceneChecked(const FScenarioId& Id) const;

private:
	/**
	* Map of scenes to all versions of their information.
	*/
	TMultiMap<FScenarioId, FVisualScenarioInfo> Versions;

};
