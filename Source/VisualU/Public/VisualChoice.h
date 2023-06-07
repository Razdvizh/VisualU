// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VisualSprite.h"
#include "VisualChoice.generated.h"

class UDataTable;
class UVisualScene;
/**
 * 
 */
UCLASS()
class VISUALU_API UVisualChoice : public UVisualSprite
{
	GENERATED_BODY()
	
public:
	UVisualChoice(const FObjectInitializer& ObjectInitializer);

	~UVisualChoice() noexcept;
	
	virtual void AssignExpressions(const TArray<TSoftObjectPtr<UPaperFlipbook>>& InExpressions) override;

	UFUNCTION(BlueprintCallable, Category = "Visual Choice")
	void Choose(UDataTable* Branch) const;

protected:
	virtual void NativeOnInitialized() override;

private:
	UVisualScene* VisualScene;
};
