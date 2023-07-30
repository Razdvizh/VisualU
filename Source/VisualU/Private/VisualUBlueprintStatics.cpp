// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualUBlueprintStatics.h"
#include "PaperSprite.h"

UTexture2D* UVisualUBlueprintStatics::GetSpriteTexture(UPaperSprite* Sprite)
{
	return Sprite->GetBakedTexture();
}
