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

void UVisualChoice::AssignExpressions(const TArray<TSoftObjectPtr<UPaperFlipbook>>& InExpressions)
{
	Super::AssignExpressions(InExpressions);
}

void UVisualChoice::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	VisualScene = UVisualScene::Get();
}

void UVisualChoice::Choose(UDataTable* Branch) const
{
	TArray<FScenario*> Scenes;
	Branch->GetAllRows(TEXT("VisualChoice.cpp(34)"), Scenes);
	if (VisualScene)
	{
		VisualScene->NativeToBranch(Scenes);
	}
}