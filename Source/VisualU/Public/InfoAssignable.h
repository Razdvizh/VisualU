// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InfoAssignable.generated.h"

struct FVisualImageInfo;
struct FScenarioInfo;
struct FVisualBackgroundImageInfo;

USTRUCT(BlueprintInternalUseOnly)
struct FVisualInfo
{
	GENERATED_USTRUCT_BODY()

	FVisualInfo() {};
	virtual ~FVisualInfo() {};

	virtual FString ToString() const PURE_VIRTUAL(FVisualInfo::ToString, return FString(););
};



UINTERFACE(MinimalAPI)
class UInfoAssignable : public UInterface
{
	GENERATED_BODY()
};

class IInfoAssignable
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void AssignVisualImageInfo(const FVisualImageInfo& Info) PURE_VIRTUAL(IInfoAssignable::AssignVisualImageInfo, );

	virtual void AssignScenarioInfo(const FScenarioInfo& Info) PURE_VIRTUAL(IInfoAssignable::AssignScenarioInfo, );

	virtual void AssignVisualBackgroundImageInfo(const FVisualBackgroundImageInfo& Info) PURE_VIRTUAL(IInfoAssignable::AssignVisualBackgroundImageInfo, );

};
