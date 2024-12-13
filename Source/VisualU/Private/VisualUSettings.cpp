// Copyright (c) 2024 Evgeny Shustov


#include "VisualUSettings.h"
#include "Scenario.h"

// None, Character, Choice
constexpr int32 CustomEnumOffset = 3;		

UVisualUSettings::UVisualUSettings(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer),
	FirstDataTable(),
	TransitionMPC(),
	TransitionDuration(0.f),
	AParameterName(TEXT("Transition 1")),
	BParameterName(TEXT("Transition 2")),
#if WITH_EDITORONLY_DATA
	ScenarioFlagsNameOverrides()
#endif
{
}

void UVisualUSettings::PostInitProperties()
{
	Super::PostInitProperties();

#if WITH_EDITOR
	ApplyOverrides();
#endif
}

#if WITH_EDITOR
void UVisualUSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.GetMemberPropertyName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UVisualUSettings, ScenarioFlagsNameOverrides))
	{
		ApplyOverrides();
	}
}

void UVisualUSettings::ApplyOverrides()
{
	UEnum* ScenarioFlags = StaticEnum<EScenarioMetaFlags>();
	check(ScenarioFlags);

	for (int32 i = 0; i < ScenarioFlagsNameOverrides.Num(); i++)
	{
		const FString NameOverride = ScenarioFlagsNameOverrides[i];
		ScenarioFlags->SetMetaData(TEXT("DisplayName"), *NameOverride, i + CustomEnumOffset);
	}
}

void UVisualUSettings::ResetOverrides()
{
	UEnum* ScenarioFlags = StaticEnum<EScenarioMetaFlags>();
	check(ScenarioFlags);

	const int32 NumCustomEnums = 5;
	ScenarioFlagsNameOverrides.Empty(NumCustomEnums);

	for (int32 i = 0; i < NumCustomEnums; i++)
	{
		const FString DefaultCustomEnumName = ScenarioFlags->GetAuthoredNameStringByIndex(i + CustomEnumOffset)
													.Replace(TEXT("_"), TEXT(" "));
		ScenarioFlagsNameOverrides.Add(DefaultCustomEnumName);
		ScenarioFlags->SetMetaData(TEXT("DisplayName"), *DefaultCustomEnumName, i + CustomEnumOffset);
	}

	TryUpdateDefaultConfigFile();
}
#endif
