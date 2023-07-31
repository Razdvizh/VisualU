
// Fill out your copyright notice in the Description page of Project Settings.


#include "ME_TextureSampleParameter2D.h"

#define LOCTEXT_NAMESPACE "Visual U"

UME_TextureSampleParameter2D::UME_TextureSampleParameter2D(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), bIsSecondTransition(false)
{
	ParameterName = TEXT("Transition 1");
}

#if WITH_EDITOR
	void UME_TextureSampleParameter2D::GetCaption(TArray<FString>& OutCaptions) const
	{
		OutCaptions.Emplace(TEXT("Background Transition"));

		if (bIsSecondTransition)
		{
			OutCaptions.Emplace(LOCTEXT("UME_TextureSampleParameter2D_SecondTransition", "Transition 2").ToString());
		}
		else
		{
			OutCaptions.Emplace(LOCTEXT("UME_TextureSampleParameter2D_FirstTransition", "Transition 1").ToString());
		}
	}
	void UME_TextureSampleParameter2D::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
	{
		if (bIsSecondTransition)
		{
			ParameterName = TEXT("Transition 2");
		}
		else
		{
			ParameterName = TEXT("Transition 1");
		}

		Super::PostEditChangeProperty(PropertyChangedEvent);
	}
#endif

#undef LOCTEXT_NAMESPACE
