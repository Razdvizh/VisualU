
// Fill out your copyright notice in the Description page of Project Settings.


#include "ME_TransitionParameter2D.h"
#include "VisualUSettings.h"

UME_TransitionParameter2D::UME_TransitionParameter2D(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), 
	bIsSecondTransition(false)
{
	const UVisualUSettings* VisualSettings = GetDefault<UVisualUSettings>();
	ParameterName = VisualSettings->AParameterName;
	AParameterName = VisualSettings->AParameterName;
	BParameterName = VisualSettings->BParameterName;
}

#if WITH_EDITOR
	void UME_TransitionParameter2D::GetCaption(TArray<FString>& OutCaptions) const
	{
		OutCaptions.Emplace(TEXT("Background Transition"));

		if (bIsSecondTransition)
		{
			OutCaptions.Emplace(BParameterName.ToString());
		}
		else
		{
			OutCaptions.Emplace(AParameterName.ToString());
		}
	}
	void UME_TransitionParameter2D::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
	{
		if (bIsSecondTransition)
		{
			ParameterName = BParameterName;
		}
		else
		{
			ParameterName = AParameterName;
		}

		Super::PostEditChangeProperty(PropertyChangedEvent);
	}
#endif
