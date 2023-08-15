// Fill out your copyright notice in the Description page of Project Settings.


#include "BackgroundVisualImage.h"
#include "SBackgroundVisualImage.h"

TSharedRef<SWidget> UBackgroundVisualImage::RebuildWidget()
{
	VisualImageSlate = SNew(SBackgroundVisualImage);

	return VisualImageSlate.ToSharedRef();
}
