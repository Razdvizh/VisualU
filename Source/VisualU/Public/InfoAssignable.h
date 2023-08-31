// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InfoAssignable.generated.h"

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
	virtual void AssignInfo(const FVisualInfo& Info) PURE_VIRTUAL(IInfoAssignable::AssignInfo, );

	virtual void GetInfo(FVisualInfo& OutInfo) const PURE_VIRTUAL(IInfoAssignable::GetInfo, );
};
