// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VisualUSettings.generated.h"

class UDataTable;

UENUM(BlueprintType)
enum class EVisualTextAction : uint8
{
	None,
	Break,
};

/**
 * 
 */
UCLASS(config = "Plugins", defaultconfig, meta = (DisplayName = "Visual U"))
class VISUALU_API UVisualUSettings : public UObject
{
	GENERATED_BODY()

public:
	UVisualUSettings(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Visual Scene", meta = (ToolTip = "Data Table that contains the first scene"))
	TSoftObjectPtr<UDataTable> FirstDataTable;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Visual Text Block", meta = (ToolTip = "A mapping of the metacharacter and the corresponding action that Visual Text Block should take"))
	TMap<FString, EVisualTextAction> Actions;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Visual Text Block", meta = (ToolTip = "A character which, in a pair, would enclose a metacharacter"))
	FString PairCharacter;
};
