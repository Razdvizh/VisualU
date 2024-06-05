// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualControllerComponent.h"
#include "VisualController.h"

// Sets default values for this component's properties
UVisualControllerComponent::UVisualControllerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

UVisualController* UVisualControllerComponent::GetVisualController_Implementation() const
{
	return VisualController;
}

// Called when the game starts
void UVisualControllerComponent::BeginPlay()
{
	Super::BeginPlay();

}
