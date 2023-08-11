// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules\ModuleManager.h"

class FPaperSpriteAssetTypeActions;

class FVisualUEditorModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

private:
	TSharedPtr<FPaperSpriteAssetTypeActions> VisualPaperSpriteAssetTypeActions;

	EAssetTypeCategories::Type VisualAssetsCategory;
};
