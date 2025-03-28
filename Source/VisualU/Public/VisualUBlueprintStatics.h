// Copyright (c) 2024 Evgeny Shustov

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VisualUBlueprintStatics.generated.h"

class UTexture2D;
class UPaperSprite;
class UVisualVersioningSubsystem;
class UVisualController;
class UDataTable;
class FArchive;
struct FScenario;
struct FAssetData;

/**
* Blueprint library with utility functions that could be helpful while
* developing blueprints with VisualU plugin.
*/
UCLASS()
class VISUALU_API UVisualUBlueprintStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	* @param Sprite sprite to get texture from
	* @return rendered texture of the sprite
	*/
	UFUNCTION(BlueprintCallable, Category = "VisualU|Paper Sprite", meta = (ToolTip = "Get the texture of sprite that should be rendered"))
	static UTexture2D* GetSpriteTexture(UPaperSprite* Sprite);

	/**
	* @param Scenario scene to get hash from
	* @return hash of the provided scenario
	*/
	UFUNCTION(BlueprintCallable, Category = "VisualU|Scenario")
	static int64 GetScenarioHash(const FScenario& Scenario);

	/**
	* Development only.
	* 
	* Outputs a friendly representation of scene asset data to the log.
	*/
	UFUNCTION(BlueprintCallable, Category = "VisualU|Scenario", meta = (DevelopmentOnly, Keywords = "Scene data", ToolTip = "Outputs a friendly representation of scene asset data to the log"))
	static void PrintScenesData();

	/**
	* Loads previously saved VisualU contents from provided filename.
	* 
	* @param VersioningSubsystem subsystem to get the loaded data
	* @param VisualController controller to get the loaded data
	* @param UserIndex user performing the load
	* @param Filename save file to load contents from
	* 
	* @return whether or not loading succeeded
	*/
	UFUNCTION(BlueprintCallable, Category = "VisualU|Serialization", meta = (ToolTip = "Loads previously saved VisualU contents from provided filename."))
	static bool LoadVisualU(UVisualVersioningSubsystem* VersioningSubsystem, UVisualController* VisualController, int32 UserIndex, const FString& Filename);

	/**
	* Saves VisualU contents to provided filename.
	* 
	* @param VersioningSubsystem subsystem to save
	* @param VisualController controller to save
	* @param UserIndex user performing the save
	* @param Filename file to store VisualU content
	* 
	* @return whether or not saving succeeded
	*/
	UFUNCTION(BlueprintCallable, Category = "VisualU|Serialization", meta = (ToolTip = "Saves VisualU contents to provided filename."))
	static bool SaveVisualU(UVisualVersioningSubsystem* VersioningSubsystem, UVisualController* VisualController, int32 UserIndex, const FString& Filename);

	/**
	* Requests provided data table in the controller.
	* 
	* @param Controller controller that will request provided data table
	* @param DataTable new controller node
	* @return result of data table request
	* 
	* @see UVisualController::RequestNode()
	*/
	UFUNCTION(BlueprintCallable, Category = "VisualU|Choice", meta = (ToolTip = "Requests provided data table in the controller."))
	static bool Choose(UVisualController* Controller, const UDataTable* DataTable);

private:
	/**
	* Gathers asset data of all data tables based on FScenario.
	* 
	* @param OutData data of scene data tables
	*/
	static void GetScenesData(TArray<FAssetData>& OutData);

	/**
	* Serializes subsystem and controller to the provided archive.
	* 
	* @param Ar Archive to handle serialization
	* @param VersioningSubsystem subsystem to be serialized
	* @param VisualController controller to be serialized
	*/
	static void SerializeVisualU(FArchive& Ar, UVisualVersioningSubsystem* VersioningSubsystem, UVisualController* VisualController);

};
