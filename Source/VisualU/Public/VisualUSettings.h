// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VisualUSettings.generated.h"

class UDataTable;
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

};
