// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualSprite.h"
#include "Blueprint/WidgetTree.h"
#include "VisualDefaults.h"
#include "VisualImage.h"
#include "PaperFlipbook.h"
#include "Animation/WidgetAnimation.h"

UVisualSprite::UVisualSprite(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UVisualSprite::AssignSpriteInfo(const TArray<FVisualImageInfo>& InInfo)
{
	if (InInfo.IsEmpty())
	{
		return;
	}
	TArray<UWidget*> ChildWidgets;
	WidgetTree->GetChildWidgets(GetRootWidget(), ChildWidgets);

	int i, j;
	for (i = j = 0; i < ChildWidgets.Num(); i++)
	{
		UWidget* Child = ChildWidgets[i];
		if (Child->IsA<UVisualImage>()) //< Maybe use higher level of abstraction a.k.a UVisualImageBase?
		{
			UVisualImage* ChildImage = Cast<UVisualImage>(Child);
			FVisualImageInfo VisualImageInfo = InInfo[j];

			ChildImage->AssignVisualImageInfo(VisualImageInfo);
			j++;
		}
	}
}