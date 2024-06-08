// Fill out your copyright notice in the Description page of Project Settings.


#include "SpriteStructCustomization.h"
#include "IDetailChildrenBuilder.h"
#include "DetailLayoutBuilder.h"
#include "PropertyHandle.h"
#include "DetailWidgetRow.h"
#include "Scenario.h"

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
	
}

#undef LOCTEXT_NAMESPACE
