// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualUSettings.h"

UVisualUSettings::UVisualUSettings(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer), 
	PairCharacter(),
	AParameterName(TEXT("Transition 1")),
	BParameterName(TEXT("Transition 2"))
{
}
