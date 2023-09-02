// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InfoAssignable.generated.h"

struct FVisualImageInfo;
struct FScenarioInfo;
struct FVisualBackgroundImageInfo;

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

};

USTRUCT(BlueprintInternalUseOnly)
struct FVisualInfo
{
	GENERATED_USTRUCT_BODY()

	FVisualInfo() {};
	virtual ~FVisualInfo() {};

	virtual void Accept(IInfoAssignable* Visitor) const PURE_VIRTUAL(FVisualInfo::Accept, );

	virtual FString ToString() const PURE_VIRTUAL(FVisualInfo::ToString, return FString(););
};
