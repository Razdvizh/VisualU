// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VisualSprite.h"
#include "VisualChoice.generated.h"

class UDataTable;
class UVisualScene;

/// <summary>
/// Sprite that represents a choice between branches.
/// </summary>
UCLASS()
class VISUALU_API UVisualChoice : public UVisualSprite
{
	GENERATED_BODY()
	
public:
	UVisualChoice(const FObjectInitializer& ObjectInitializer);

	/// <summary>
	/// Setter for <see cref="UVisualScene">Visual Scene</see> if it wasn't provided during construction.
	/// </summary>
	/// <param name="Scene">Active Visual Scene</param>
	UFUNCTION(BlueprintCallable, Category = "Visual Scene")
	void SetVisualScene(UVisualScene* Scene);
	
	/// \copydoc UVisualSprite
	virtual void AssignVisualImageInfo(const TArray<FVisualImageInfo>& InInfo) override;

	/// <summary>
	/// Sets the provided branch as active for operations inside the <see cref="UVisualScene">Visual Scene</see>.
	/// </summary>
	/// <param name="Branch">Branch to operate on</param>
	UFUNCTION(BlueprintCallable, Category = "Visual Choice")
	void Choose(const UDataTable* Branch) const;

protected:
	virtual void NativeOnInitialized() override;
	
	/// <summary>
	/// Pointer to the active <see cref="UVisualScene">Visual Scene</see>.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual Scene", meta = (ExposeOnSpawn = true))
	UVisualScene* VisualScene;
};
