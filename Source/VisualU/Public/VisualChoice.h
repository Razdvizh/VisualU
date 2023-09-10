// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VisualSprite.h"
#include "VisualChoice.generated.h"

class UDataTable;
class UVisualScene;

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
	/// Setter for <see cref="UVisualScene">Visual Scene</see> if it wasn't provided during construction.
	/// </summary>
	/// <param name="Scene">Active Visual Scene</param>
	UFUNCTION(BlueprintCallable, Category = "Visual Scene", meta = (ToolTip = "Setter for the Visual Scene in case it wasn't provided during construction"))
	void SetVisualScene(UVisualScene* Scene);
	
	/// \copydoc UVisualSprite
	virtual void AssignSpriteInfo(const TArray<FVisualImageInfo>& InInfo) override;

	/// <summary>
	/// Sets the provided node as active for operations inside the <see cref="UVisualScene">Visual Scene</see>.
	/// </summary>
	/// <param name="node">node to operate on</param>
	UFUNCTION(BlueprintCallable, Category = "Visual Choice", meta = (ToolTip = "Sets the provided node as active for operations inside Visual Secne"))
	void Choose(const UDataTable* Node) const;

protected:
	/// <summary>
	/// Pointer to the active <see cref="UVisualScene">Visual Scene</see>.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual Scene", meta = (ExposeOnSpawn = true, ToolTip = "Pointer to the active Visual Scene"))
	UVisualScene* VisualScene;
};
