// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualPaperSpriteFactory.h"
#include "VisualU/Public/VisualPaperSprite.h"

UVisualPaperSpriteFactory::UVisualPaperSpriteFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SupportedClass = UVisualPaperSprite::StaticClass();
	bCreateNew = true;
}

UObject* UVisualPaperSpriteFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UVisualPaperSprite>(InParent, Name, Flags | RF_Transactional);
}
