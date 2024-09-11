// Copyright (c) 2024 Evgeny Shustov

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VisualUBlueprintStatics.generated.h"

class UPaperSprite;
class UVisualController;
class UDataTable;

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
	* Development only.
	* 
	* Outputs a friendly representation of scene asset data to the log.
	*/
	UFUNCTION(BlueprintCallable, Category = "VisualU|Scenario", meta = (DevelopmentOnly, ToolTip = "Outputs a friendly representation of scene asset data to the log"))
	static void PrintScenesData();

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
};
