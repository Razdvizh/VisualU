// Copyright (c) 2024 Evgeny Shustov


#include "ME_TransitionParameter2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "VisualUSettings.h"

UME_TransitionParameter2D::UME_TransitionParameter2D(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), 
	bIsSecondTransition(false)
{
	const UVisualUSettings* VisualSettings = GetDefault<UVisualUSettings>();
	ParameterName = VisualSettings->AParameterName;
	AParameterName = VisualSettings->AParameterName;
	BParameterName = VisualSettings->BParameterName;
	SamplerSource = ESamplerSourceMode::SSM_Wrap_WorldGroupSettings;
}

UMaterialInstanceDynamic* UME_TransitionParameter2D::GetTransitionMaterial(UMaterialInstanceDynamic* MaterialToChange, const TMap<FName, UTexture*>& Params)
{
	for (auto It = Params.CreateConstIterator(); It; ++It)
	{
		MaterialToChange->SetTextureParameterValue(It.Key(), It.Value());
	}

	return MaterialToChange;
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
