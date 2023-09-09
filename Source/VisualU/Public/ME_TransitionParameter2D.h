// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "ME_TransitionParameter2D.generated.h"

struct FPropertyChangedEvent;

/// <summary>
/// Material node for transition materials.
/// </summary>
/// <remarks>
/// Represents either first (currently rendered), or second (final target) sprite in the material asset which serves as transition effect.
/// Parameters are immutable in the node and can only be switched by <see cref="UME_TransitionParameter2D::bIsSecondTransition"/>.
/// Parameter names, however can be changed in the <see cref="UVisualSettings">Visual Settings</see> for all transition parameter nodes.
/// </remarks>
UCLASS()
class VISUALU_API UME_TransitionParameter2D : public UMaterialExpressionTextureSampleParameter2D
{
	GENERATED_UCLASS_BODY()

	/// <summary>
	/// Whether or not this parameter node represents final sprite that should be visualized after transition ends.
	/// </summary>
	UPROPERTY(EditAnywhere, Category = "Visual U")
	bool bIsSecondTransition;

	/// <summary>
	/// Transition begins with this sprite.
	/// </summary>
	FName AParameterName;

	/// <summary>
	/// Transition ends with this sprite.
	/// </summary>
	FName BParameterName;

#if WITH_EDITOR
	/// <summary>
	/// Change captions of the node.
	/// </summary>
	/// <param name="OutCaptions">Changed captions of the node</param>
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;

	/// <summary>
	/// Parameter node can't be renamed.
	/// </summary>
	virtual bool CanRenameNode() const override { return false; }

	/// <summary>
	/// Apply changes to the node made in editor.
	/// </summary>
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
