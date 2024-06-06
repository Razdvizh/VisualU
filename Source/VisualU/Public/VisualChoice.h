// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VisualSprite.h"
#include "VisualChoice.generated.h"

class UDataTable;
class UVisualScene;
class UVisualController;

/// <summary>
/// Sprite that represents a choice between nodes.
/// </summary>
UCLASS(meta = (ToolTip = "Sprite that represents a choice between nodes"))
class VISUALU_API UVisualChoice : public UVisualSprite
{
	GENERATED_BODY()
	
public:
	UVisualChoice(const FObjectInitializer& ObjectInitializer);

	/// <summary>
	/// Sets the provided node as active for operations inside the <see cref="UVisualScene">Visual Scene</see>.
	/// </summary>
	/// <param name="node">node to operate on</param>
	UFUNCTION(BlueprintCallable, Category = "Visual Choice", meta = (ToolTip = "Sets the provided node as active for operations inside Visual Scene."))
	void LegacyChoose(UVisualScene* Scene, const UDataTable* Node) const;

	UFUNCTION(BlueprintCallable, Category = "Visual Choice")
	void Choose(UVisualController* Controller, const UDataTable* Node) const;

};
