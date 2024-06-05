// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VisualControllerInterface.h"
#include "VisualControllerComponent.generated.h"

class UVisualController;

/*
* Provides convenient access to @see UVisualController
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VISUALU_API UVisualControllerComponent : public UActorComponent, public IVisualControllerInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVisualControllerComponent();

	virtual UVisualController* GetVisualController_Implementation() const override final;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UVisualController* VisualController;
};
