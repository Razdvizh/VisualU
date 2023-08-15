// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SVisualImage.h"

class VISUALU_API SBackgroundVisualImage : public SVisualImage
{
private:
	virtual void PostSlateDrawElementExtension() const override final;
};
