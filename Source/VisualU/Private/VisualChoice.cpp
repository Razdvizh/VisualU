// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualChoice.h"
#include "Engine/DataTable.h"
#include "Scenario.h"
#include "VisualScene.h"

UVisualChoice::UVisualChoice(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void UVisualChoice::SetVisualScene(UVisualScene* Scene)
{
	VisualScene = Scene;
}

void UVisualChoice::AssignSpriteInfo(const TArray<FVisualImageInfo>& InInfo)
{
	Super::AssignSpriteInfo(InInfo);
}

void UVisualChoice::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UVisualChoice::Choose(const UDataTable* Node) const
{
	checkf(VisualScene, TEXT("Visual Scene is invalid. Use SetVisualScene() first"));
	VisualScene->ToNode(Node);
}