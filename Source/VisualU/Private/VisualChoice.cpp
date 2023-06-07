// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualChoice.h"
#include "Engine/DataTable.h"
#include "Scenario.h"
#include "VisualScene.h"

UVisualChoice::UVisualChoice(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

UVisualChoice::~UVisualChoice() noexcept
{
	
}

void UVisualChoice::SetVisualScene(UVisualScene* Scene)
{
	VisualScene = Scene;
}

void UVisualChoice::AssignExpressions(const TArray<TSoftObjectPtr<UPaperFlipbook>>& InExpressions)
{
	Super::AssignExpressions(InExpressions);
}

void UVisualChoice::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UVisualChoice::Choose(UDataTable* Branch) const
{
	TArray<FScenario*> Scenes;
	Branch->GetAllRows(TEXT("VisualChoice.cpp(37)"), Scenes);
	
	checkf(VisualScene, TEXT("Visual Scene is invalid. Use SetVisualScene() first"));
	VisualScene->NativeToBranch(Scenes);
}