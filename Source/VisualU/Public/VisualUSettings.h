// Copyright (c) 2024 Evgeny Shustov

#pragma once

#include "CoreMinimal.h"
#include "VisualUSettings.generated.h"

class UDataTable;
class UMaterialParameterCollection;

/**
* Global plugin settings.
* Can be found under 'Plugins' category in project settings.
* VisualU settings are written to the 'DefaultPlugins.ini' config file.
* 
* @see FVisualUModule for registration details
*/
UCLASS(config = "Plugins", defaultconfig, meta = (DisplayName = "VisualU"))
class VISUALU_API UVisualUSettings : public UObject
{
	GENERATED_BODY()

public:
	UVisualUSettings(const FObjectInitializer& ObjectInitializer);

	/**
	* Applies scenario flags name overrides after config is loaded.
	* 
	* @see UVisualUSettings::ApplyOverrides
	*/
	virtual void PostInitProperties() override;

#if WITH_EDITOR
	/**
	* Editor only.
	* 
	* Applies scenario flags name overrides when array of names is changed.
	* 
	* @see UVisualUSettings::ApplyOverrides
	* 
	* @param PropertyChangedEvent data regarding property change event. 
	*/
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	/**
	* Editor only.
	* 
	* Applies scenario meta flags name overrides.
	* Only affects metadata.
	* 
	* @see UVisualUSettings::ScenarioFlagsNameOverrides
	*/
	void ApplyOverrides();

	/**
	* Editor only.
	* 
	* Resets name overrides of scenario flags to plugin default values.
	*/
	void ResetOverrides();
#endif

	/**
	* Fist node of UVisualController.
	* Must be valid for proper initialization of the controller.
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Visual Controller", meta = (ToolTip = "Data Table that contains the first scene"))
	TSoftObjectPtr<UDataTable> FirstDataTable;

	/**
	* Material parameter collection used for transition material.
	* First scalar parameter from this collection will be used
	* to indicate progress of transition for materials.
	* 
	* @see UME_TransitionParameter2D
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Visual Controller|Transition", meta = (DisplayName = "Transition Material Parameter Collection", ToolTip = "First scalar parameter from this collection will be used to indicate progress of transition for materials"))
	TSoftObjectPtr<UMaterialParameterCollection> TransitionMPC;

	/**
	* Duration, in seconds, of the transition between scenes
	* 
	* @see UVisualRenderer::Transition
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Visual Controller|Transition", meta = (UIMin = 0.f, ClampMin = 0.f, UIMax = 5.f, ClampMax = 5.f, ToolTip = "Duration, in seconds, of the transition between scenes"))
	float TransitionDuration;

	/**
	* First parameter name for UME_TransitionParameter2D node.
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Visual Controller|Transition")
	FName AParameterName;

	/**
	* Second parameter name for UME_TransitionParameter2D node.
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Visual Controller|Transition")
	FName BParameterName;

#if WITH_EDITORONLY_DATA
private:
	/**
	* Defines user names for scenario meta flags custom enums.
	* Only affects enum metadata.
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, EditFixedSize, Category = "Editor", meta = (EditFixedOrder, AllowPrivateAccess = true))
	TArray<FString> ScenarioFlagsNameOverrides;
#endif

};
