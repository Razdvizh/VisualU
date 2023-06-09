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

void UVisualSprite::AssignVisualImageInfo(const TArray<FVisualImageInfo>& InInfo)
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
		if (Child->IsA<UVisualImage>())
		{
			UVisualImage* ChildImage = Cast<UVisualImage>(Child);
			FVisualImageInfo VisualImageInfo = InInfo[j];

			ChildImage->SetFlipbookAsync(VisualImageInfo.Expression);
			ChildImage->SetColorAndOpacity(VisualImageInfo.ColorAndOpacity);
			ChildImage->SetDesiredScale(VisualImageInfo.DesiredScale);
			ChildImage->SetMirrorScale(VisualImageInfo.MirrorScale);
			ChildImage->SetAnimate(VisualImageInfo.bAnimate);
			ChildImage->SetFrameIndex(VisualImageInfo.FrameIndex);
			j++;
		}
	}
}