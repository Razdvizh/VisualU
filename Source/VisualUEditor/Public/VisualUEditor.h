// Copyright (c) 2024 Evgeny Shustov

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
* VisualU plugin editor module.
*/
class FVisualUEditorModule : public IModuleInterface
{
	/**
	* Registers custom property layout for FSprite.
	*/
	virtual void StartupModule() override;

	/**
	* Unregisters custom property layout for FSprite.
	*/
	virtual void ShutdownModule() override;

};