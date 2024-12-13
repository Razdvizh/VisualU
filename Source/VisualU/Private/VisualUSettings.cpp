// Copyright (c) 2024 Evgeny Shustov


#include "VisualUSettings.h"

UVisualUSettings::UVisualUSettings(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer),
	FirstDataTable(),
	TransitionMPC(),
	TransitionDuration(0.f),
	AParameterName(TEXT("Transition 1")),
	BParameterName(TEXT("Transition 2"))
{
#if WITH_EDITORONLY_DATA
	ScenarioFlagsNameOverrides = TArray<FString>();
#endif
}
