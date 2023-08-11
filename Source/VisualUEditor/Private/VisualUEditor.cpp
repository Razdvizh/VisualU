// Copyright Epic Games, Inc. All Rights Reserved.

#include "VisualUEditor.h"
#include "AssetToolsModule.h"
#include "PaperSpriteAssetTypeActions.h"

#define LOCTEXT_NAMESPACE "FVisualUEditorModule"

void FVisualUEditorModule::StartupModule()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	VisualAssetsCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("")), LOCTEXT("FVisualUAssetCategory", "VisualU"));
	VisualPaperSpriteAssetTypeActions = MakeShared<FPaperSpriteAssetTypeActions>(VisualAssetsCategory);
	AssetTools.RegisterAssetTypeActions(VisualPaperSpriteAssetTypeActions.ToSharedRef());
}

void FVisualUEditorModule::ShutdownModule()
{
	IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();

	AssetTools.UnregisterAssetTypeActions(VisualPaperSpriteAssetTypeActions.ToSharedRef());
	VisualPaperSpriteAssetTypeActions.Reset();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVisualUEditorModule, VisualUEditor)