// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

/// \todo refactor, name is ambiguous
class FPaperSpriteAssetTypeActions : public FAssetTypeActions_Base
{
public:
	FPaperSpriteAssetTypeActions(EAssetTypeCategories::Type AssetCategory);

	virtual UClass* GetSupportedClass() const override;
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual uint32 GetCategories() override;

private:
	EAssetTypeCategories::Type VisualAssetCategory;

};
