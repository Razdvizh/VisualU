// Copyright (c) 2024 Evgeny Shustov

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

/**
 * Adds custom button to the Visual Settings.
 */
class FVisualUSettingsCustomization : public IDetailCustomization
{
public:
	FVisualUSettingsCustomization();

	VISUALUEDITOR_API static TSharedRef<IDetailCustomization> MakeInstance();

	/**
	* Adds custom button to the Visual Settings.
	* 
	* @param DetailBuilder handle to perform customization
	*/
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};
