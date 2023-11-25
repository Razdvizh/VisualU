// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class ISettingsSection;
class UVisualUSettings;

/// <summary>
/// VisualU custom log category.
/// </summary>
DECLARE_LOG_CATEGORY_EXTERN(LogVisualU, Display, All);

/// <summary>
/// VisualU plugin module.
/// </summary>
/// <remarks>
/// Registers and owns the <see cref="UVisualUSettings">settings</see>.
/// Provides custom log category for VisualU systems.
/// </remarks>
class FVisualUModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

	/// <summary>
	/// Getter for the <see cref="UVisualUSettings"/>.
	/// Might be <c>nullptr</c> if called before the module is loaded. Editor only.
	/// </summary>
	/// <returns>VisualU Settings</returns>
#if WITH_EDITOR
	const UVisualUSettings* GetVisualSettings() const;
#endif

private:
	/// <summary>
	/// Represents the VisualU settings section.
	/// </summary>
	/// <remarks>
	/// Owner of the <see cref="UVisualUSettings"/>.
	/// </remarks>
#if WITH_EDITOR
	TSharedPtr<ISettingsSection> SettingsSection;
#endif
};
