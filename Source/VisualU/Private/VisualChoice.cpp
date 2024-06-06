// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualChoice.h"
#include "Engine/DataTable.h"
#include "VisualScene.h"
#include "VisualController.h"

UVisualChoice::UVisualChoice(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	
}

void UVisualChoice::LegacyChoose(UVisualScene* Scene, const UDataTable* Node) const
{
	check(Scene);
	Scene->ToNode(Node);
}

void UVisualChoice::Choose(UVisualController* Controller, const UDataTable* Node) const
{
	check(Controller);
	Controller->ToNode(Node);
}
