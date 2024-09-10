// Copyright (c) 2024 Evgeny Shustov


#include "VisualSprite.h"
#include "Blueprint/WidgetTree.h"
#include "VisualDefaults.h"
#include "VisualImage.h"
#include "PaperFlipbook.h"
#include "Animation/WidgetAnimation.h"

UVisualSprite::UVisualSprite(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UVisualSprite::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
}

void UVisualSprite::AssignSpriteInfo(const TArray<FVisualImageInfo>& InInfo)
{
	if (!InInfo.IsEmpty())
	{
		TArray<UWidget*> ChildWidgets;
		WidgetTree->GetChildWidgets(GetRootWidget(), ChildWidgets);

		int32 i, j;
		for (i = j = 0; i < ChildWidgets.Num(); i++)
		{
			UWidget* Child = ChildWidgets[i];
			if (UVisualImage* ChildImage = Cast<UVisualImage>(Child))
			{
				checkf(InInfo.IsValidIndex(j), TEXT("There was less SpriteInfo than the sprite widget requires. You must provide an info for each Visual Image in the sprite"));
				const FVisualImageInfo& VisualImageInfo = InInfo[j];
				ChildImage->AssignVisualImageInfo(VisualImageInfo);
				j++;
			}
		}
	}
}
