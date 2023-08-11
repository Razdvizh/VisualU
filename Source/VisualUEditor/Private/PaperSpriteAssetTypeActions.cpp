// Fill out your copyright notice in the Description page of Project Settings.


#include "PaperSpriteAssetTypeActions.h"
#include "VisualU/Public/VisualPaperSprite.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FPaperSpriteAssetTypeActions::FPaperSpriteAssetTypeActions(EAssetTypeCategories::Type AssetCategory) : VisualAssetCategory(AssetCategory)
{
}

UClass* FPaperSpriteAssetTypeActions::GetSupportedClass() const
{
	return UVisualPaperSprite::StaticClass();
}

FText FPaperSpriteAssetTypeActions::GetName() const
{
	return LOCTEXT("PaperSpriteAssetTypeActionsName", "Visual Paper Sprite");
}

FColor FPaperSpriteAssetTypeActions::GetTypeColor() const
{
	return FColor::Emerald;
}

uint32 FPaperSpriteAssetTypeActions::GetCategories()
{
	return VisualAssetCategory;
}

#undef LOCTEXT_NAMESPACE