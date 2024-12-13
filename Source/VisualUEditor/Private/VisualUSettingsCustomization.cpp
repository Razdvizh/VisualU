// Copyright (c) 2024 Evgeny Shustov


#include "VisualUSettingsCustomization.h"
#include "VisualUSettings.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SButton.h"

#define LOCTEXT_NAMESPACE "VisualUEditor"

FVisualUSettingsCustomization::FVisualUSettingsCustomization() = default;

VISUALUEDITOR_API TSharedRef<IDetailCustomization> FVisualUSettingsCustomization::MakeInstance()
{
	return MakeShareable(new FVisualUSettingsCustomization());
}

void FVisualUSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UVisualUSettings>> VisualUSettingsCustomized = DetailBuilder.GetObjectsOfTypeBeingCustomized<UVisualUSettings>();
	check(!VisualUSettingsCustomized.IsEmpty());

	TWeakObjectPtr<UVisualUSettings> WeakSettings = VisualUSettingsCustomized[0];

	IDetailCategoryBuilder& EditorCategory = DetailBuilder.EditCategory("Editor");

	EditorCategory.AddCustomRow(LOCTEXT("VisualUSettingsCustomizationRowFilter", "VisualUSettings"))
		.ValueContent()
		[
			SNew(SButton)
				.Text(LOCTEXT("VisualUSettingsCustomizationResetOverridesText", "Reset Overrides"))
				.ToolTipText(LOCTEXT("VisualUSettingsCustomizationResetOverridesToolTip", "Resets name overrides of scenario flags to plugin's default values."))
				.OnClicked_Lambda([WeakSettings]
				{
					if (WeakSettings.IsValid())
					{
						WeakSettings->ResetOverrides();
					}

					return FReply::Handled();
				})
		];
}

#undef LOCTEXT_NAMESPACE
