// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VisualUSettings.generated.h"

class UDataTable;

/// <summary>
/// Actions supported by <see cref="UVisualTextBlock">Visual Text Block</see>.
/// </summary>
UENUM(BlueprintType)
enum class EVisualTextAction : uint8
{
	None, /* < No action */
	Break, /* < Stops text appearance */
};

/// <summary>
/// Global plugin settings.
/// You can find them under "Plugins" settings category.
/// </summary>
/// <remarks>
/// The class is derived directly from <c>UObject</c> and simply contains necessary data.
/// It is not a subclass of <c>UDeveloperSettings</c> due to the internal structure of the Unreal plugins.
/// Registration for engine's settings system occurs through <c>ISettingsModule</c> inside <see cref="FVisualUModule">Module struct</see>.
/// </remarks>
UCLASS(config = "Plugins", defaultconfig, meta = (DisplayName = "Visual U"))
class VISUALU_API UVisualUSettings : public UObject
{
	GENERATED_BODY()

public:
	UVisualUSettings(const FObjectInitializer& ObjectInitializer);

	/// <summary>
	/// Data Table that represents the first branch of scenarios when game starts.
	/// </summary>
	/// <remarks>
	/// Contains the first scenario that will be loaded and displayed by <see cref="UVisualScene">Visual Scene</see>;
	/// the entry point in the VisualU system.
	/// <see cref="UVisualScene">Visual Scene</see> handles the processing and constructing of the first scenario, 
	/// if the Data Table is empty, the assertion will be triggered.
	/// </remarks>
	/// <seealso cref="FScenario"/>
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Visual Scene", meta = (ToolTip = "Data Table that contains the first scenario"))
	TSoftObjectPtr<UDataTable> FirstDataTable;

	/// <summary>
	/// A mapping of the metacharacter and the corresponding action that <see cref="UVisualTextBlock">Visual Text Block</see> should take.
	/// </summary>
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Visual Text Block", meta = (ToolTip = "A mapping of the metacharacter and the corresponding action that Visual Text Block should take"))
	TMap<FString, EVisualTextAction> Actions;

	/// <summary>
	/// A character which, in a pair, would enclose a metacharacter mapped in <see cref="UVisualUSettings::Actions">Actions</see>.
	/// </summary>
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Visual Text Block", meta = (ToolTip = "A character which, in a pair, would enclose a metacharacter"))
	FString PairCharacter;

	/// <summary>
	/// First parameter name for <see cref="UME_TransitionParameter2D"/> nodes.
	/// </summary>
	/// <remarks>
	/// Transition begins with this sprite.
	/// </remarks>
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Visual Scene|Transitions")
	FName AParameterName;

	/// <summary>
	/// Second parameter name for <see cref="UME_TransitionParameter2D"/> nodes.
	/// </summary>
	/// <remarks>
	/// Transition ends with this sprite.
	/// </remarks>
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Visual Scene|Transitions")
	FName BParameterName;
};
