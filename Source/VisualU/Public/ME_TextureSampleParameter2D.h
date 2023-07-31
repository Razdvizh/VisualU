// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "ME_TextureSampleParameter2D.generated.h"

struct FPropertyChangedEvent;

/// <summary>
/// Represents first or second sprite for transition animation.
/// </summary>
/// <remarks>
/// \todo Add documentation
/// </remarks>
UCLASS()
class VISUALU_API UME_TextureSampleParameter2D : public UMaterialExpressionTextureSampleParameter2D
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = "Visual U")
	bool bIsSecondTransition;

#if WITH_EDITOR
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;

	virtual bool CanRenameNode() const override { return false; }

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
