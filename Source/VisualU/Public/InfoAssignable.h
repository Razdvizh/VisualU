// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InfoAssignable.generated.h"

struct FVisualImageInfo;
struct FVisualScenarioInfo;

UINTERFACE(MinimalAPI)
class UInfoAssignable : public UInterface
{
	GENERATED_BODY()
};

/// <summary>
/// Interface that allows classes to take data from <see cref="FVisualInfo">Visual Info</see> structures.
/// Classes can assign data from multiple structures.
/// </summary>
/// <remarks>
/// Use <see cref="IInfoAssignable::AssignVisualImageInfo"/> as an example when adding function for custom <see cref="FVisualInfo">Visual Info</see> struct.
/// </remarks> 
class IInfoAssignable
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void AssignVisualImageInfo(const FVisualImageInfo& Info) PURE_VIRTUAL(IInfoAssignable::AssignVisualImageInfo, );

	virtual void AssignScenarioVisualInfo(const FVisualScenarioInfo& Info) PURE_VIRTUAL(IInfoAssignable::AssignScenarioVisualInfo, );

};

/// <summary>
/// Base struct for Visual Info system. 
/// </summary>
/// <remarks>
/// Represents certain data which other classes can retrieve through <see cref="IInfoAssignable"/> interface.
/// To create custom Visual Info struct:
/// -# Create a struct that inherits from <c>FVisualInfo</c>
/// -# Add desired fields
/// -# Override <c>Accept</c> function to implement Visitor pattern (see <see cref="FVisualImageInfo::Acept"/> for example) 
/// -# Override <c>ToString</c> function if needed
/// Check <see cref="FVisualImageInfo"/>
USTRUCT(BlueprintInternalUseOnly)
struct FVisualInfo
{
	GENERATED_BODY()

public:
	FVisualInfo();

	virtual ~FVisualInfo();

	virtual void Accept(IInfoAssignable* Visitor) const PURE_VIRTUAL(FVisualInfo::Accept, );

	virtual FString ToString() const PURE_VIRTUAL(FVisualInfo::ToString, return FString(););
};
