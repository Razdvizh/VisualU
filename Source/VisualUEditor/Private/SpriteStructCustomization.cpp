// Fill out your copyright notice in the Description page of Project Settings.


#include "SpriteStructCustomization.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "DetailLayoutBuilder.h"
#include "PropertyHandle.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SComboButton.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Scenario.h"
#include "VisualDefaults.h"

#define LOCTEXT_NAMESPACE "Visual U Editor"

VISUALUEDITOR_API TSharedRef<IPropertyTypeCustomization> FSpriteStructCustomization::MakeInstance()
{
	return MakeShareable(new FSpriteStructCustomization());
}

void FSpriteStructCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	//No op
}

void FSpriteStructCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	TSharedPtr<IPropertyHandle> SpriteClassHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSprite, SpriteClass));
	TSharedPtr<IPropertyHandle> AnchorsHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSprite, Anchors));
	TSharedPtr<IPropertyHandle> PositionHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSprite, Position));
	TSharedPtr<IPropertyHandle> ZOrderHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSprite, ZOrder));
	TSharedPtr<IPropertyHandle> SpriteInfoHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSprite, SpriteInfo));

	ChildBuilder.AddProperty(SpriteClassHandle.ToSharedRef());
	IDetailPropertyRow& AnchorsPropertyRow = ChildBuilder.AddProperty(AnchorsHandle.ToSharedRef());
	ChildBuilder.AddProperty(PositionHandle.ToSharedRef());
	ChildBuilder.AddProperty(ZOrderHandle.ToSharedRef());
	ChildBuilder.AddProperty(SpriteInfoHandle.ToSharedRef());

	FMenuBuilder MenuBuilder(true, nullptr, nullptr, true, &FAppStyle::Get());
	const auto AddAnchorsPreset = [&MenuBuilder](
		const FText& PresetName, 
		const FText& PresetToolTip, 
		const FVisualAnchors& Value, 
		TSharedRef<IPropertyHandle> Handle)
	{
		MenuBuilder.AddMenuEntry(
			PresetName,
			PresetToolTip,
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateLambda([Value, Handle]
				{
					const FString AsString = FString::Printf(TEXT("(Minimum=(X=%f,Y=%f),Maximum=(X=%f,Y=%f))"), Value.Minimum.X, Value.Minimum.Y, Value.Maximum.X, Value.Maximum.Y);
					Handle->SetValueFromFormattedString(AsString);
				})
			)
		);
	};

	AddAnchorsPreset(LOCTEXT("FullscreenPreset", "Fullscreen"), LOCTEXT("FullscreenToolTip", "0, 0, 1, 1"), FVisualAnchors::FullScreen, AnchorsHandle.ToSharedRef());
	AddAnchorsPreset(LOCTEXT("BottomLeftPreset", "Bottom left"), LOCTEXT("BottomLeftToolTip", "0, 1, 0, 1"), FVisualAnchors::BottomLeft, AnchorsHandle.ToSharedRef());
	AddAnchorsPreset(LOCTEXT("MiddleLeftPreset", "Middle left"), LOCTEXT("MiddleLeftToolTip", "0, 0.5, 0, 0.5"), FVisualAnchors::MiddleLeft, AnchorsHandle.ToSharedRef());
	AddAnchorsPreset(LOCTEXT("TopLeftPreset", "Top left"), LOCTEXT("TopLeftToolTip", "0, 0, 0, 0"), FVisualAnchors::TopLeft, AnchorsHandle.ToSharedRef());
	AddAnchorsPreset(LOCTEXT("BottomRightPreset", "Bottom right"), LOCTEXT("BottomRightToolTip", "1, 1, 1, 1"), FVisualAnchors::BottomRight, AnchorsHandle.ToSharedRef());
	AddAnchorsPreset(LOCTEXT("MiddleRightPreset", "Middle right"), LOCTEXT("MiddleRightToolTip", "1, 0.5, 1, 0.5"), FVisualAnchors::MiddleRight, AnchorsHandle.ToSharedRef());
	AddAnchorsPreset(LOCTEXT("TopRightPreset", "Top right"), LOCTEXT("TopRightToolTip", "1, 0, 1, 0"), FVisualAnchors::TopRight, AnchorsHandle.ToSharedRef());
	AddAnchorsPreset(LOCTEXT("BottomCenterPreset", "Bottom center"), LOCTEXT("BottomCenterToolTip", "0.5, 1, 0.5, 1"), FVisualAnchors::BottomCenter, AnchorsHandle.ToSharedRef());
	AddAnchorsPreset(LOCTEXT("CenterPreset", "Center"), LOCTEXT("CenterToolTip", "0.5, 0.5, 0.5, 0.5"), FVisualAnchors::Center, AnchorsHandle.ToSharedRef());
	AddAnchorsPreset(LOCTEXT("TopCenterPreset", "Top center"), LOCTEXT("TopCenterToolTip", "0.5, 0, 0.5, 0"), FVisualAnchors::TopCenter, AnchorsHandle.ToSharedRef());
	AddAnchorsPreset(LOCTEXT("BottomHorizontalPreset", "Bottom horizontal"), LOCTEXT("BottomHorizontalToolTip", "0, 1, 1, 1"), FVisualAnchors::BottomHorizontal, AnchorsHandle.ToSharedRef());
	AddAnchorsPreset(LOCTEXT("MiddleHorizontalPreset", "Middle horizontal"), LOCTEXT("MiddleHorizontalToolTip", "0, 0.5, 1, 0.5"), FVisualAnchors::MiddleHorizontal, AnchorsHandle.ToSharedRef());
	AddAnchorsPreset(LOCTEXT("TopHorizontalPreset", "Top horizontal"), LOCTEXT("TopHorizontalToolTip", "0, 0, 1, 0"), FVisualAnchors::TopHorizontal, AnchorsHandle.ToSharedRef());
	AddAnchorsPreset(LOCTEXT("LeftVerticalPreset", "Left vertical"), LOCTEXT("LeftVerticalToolTip", "0, 0, 0, 1"), FVisualAnchors::LeftVertical, AnchorsHandle.ToSharedRef());
	AddAnchorsPreset(LOCTEXT("CenterVerticalPreset", "Center vertical"), LOCTEXT("CenterVerticalToolTip", "0.5, 0, 0.5, 1"), FVisualAnchors::CenterVertical, AnchorsHandle.ToSharedRef());
	AddAnchorsPreset(LOCTEXT("RightVerticalPreset", "Right vertical"), LOCTEXT("RightVerticalToolTip", "1, 0, 1, 1"), FVisualAnchors::RightVertical, AnchorsHandle.ToSharedRef());
	
	AnchorsPropertyRow.CustomWidget(true)
		.NameContent()
		[
			SNew(STextBlock)
				.Font(IDetailLayoutBuilder::GetDetailFont())
				.Text(LOCTEXT("Anchors", "Anchors"))
		]
		.ValueContent()
		[
			SNew(SComboButton)
				.ButtonContent()
				[
					SNew(STextBlock)
						.Text(LOCTEXT("Presets", "Presets"))
						.Font(IDetailLayoutBuilder::GetDetailFont())
				]
				.ToolTipText(LOCTEXT("PresetsToolTip", "Presets"))
				.MenuContent()
				[
					MenuBuilder.MakeWidget()
				]
		];
}

#undef LOCTEXT_NAMESPACE
