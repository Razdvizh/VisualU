// Copyright (c) 2024 Evgeny Shustov

#pragma once

#include "CoreMinimal.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "ME_TransitionParameter2D.generated.h"

class UMaterialInstanceDynamic;
struct FPropertyChangedEvent;

/**
* Material node for transition materials.
* Represents either first (currently rendered), or second (final target) sprite
* in the material asset which serves as transition effect.
* Parameters are immutable in the node and can only be
* switched by UME_TransitionParameter2D::bIsSecondTransition.
* Parameter names, however can be changed in UVisualSettings
* for all transition parameter nodes.
*/
UCLASS()
class VISUALU_API UME_TransitionParameter2D : public UMaterialExpressionTextureSampleParameter2D
{
	GENERATED_BODY()

public:
	UME_TransitionParameter2D(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	* Applies given parameters to the provided dynamic material.
	* 
	* @param MaterialToChange material that should render transition
	* @param Params parameters for the material instance
	* @return dynamic material instance with applied parameters
	*/
	static UMaterialInstanceDynamic* GetTransitionMaterial(UMaterialInstanceDynamic* MaterialToChange, const TMap<FName, UTexture*>& Params);

	/**
	* Whether or not this parameter node represents final sprite
	* that should be visualized after transition ends.
	*/
	UPROPERTY(EditAnywhere, Category = "VisualU")
	bool bIsSecondTransition;

	/**
	* Transition begins with this sprite.
	*/
	FName AParameterName;

	/**
	* Transition ends with this sprite.
	*/
	FName BParameterName;

#if WITH_EDITOR
	/**
	* Changes captions of the node.
	* 
	* @param OutCaptions Changed captions of the node
	*/
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;

	/**
	* Parameter node can't be renamed.
	*/
	virtual bool CanRenameNode() const override { return false; }

	/**
	* Applies changes to the node made in editor.
	* 
	* @param PropertyChangedEvent contains the changed property details
	*/
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
