// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VisualImage.h"
#include "BackgroundVisualImage.generated.h"

class SBackgroundVisualImage;

UCLASS()
class UBackgroundVisualImage final : public UVisualImage
{
	GENERATED_BODY()

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

};
