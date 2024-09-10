// Copyright (c) 2024 Evgeny Shustov

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

/**
* Interface that allows classes to take data from FVisualInfo structures.
* Classes can assign data from multiple structures.
* 
* Use FVisualImageInfo::AssignVisualImageInfo() as an example
* when adding function
* for custom FVisualInfo struct.
*/
class IInfoAssignable
{
	GENERATED_BODY()

public:
	virtual void AssignVisualImageInfo(const FVisualImageInfo& Info) PURE_VIRTUAL(IInfoAssignable::AssignVisualImageInfo, );

	virtual void AssignScenarioVisualInfo(const FVisualScenarioInfo& Info) PURE_VIRTUAL(IInfoAssignable::AssignScenarioVisualInfo, );

};

/**
* Base struct for assignable info.
* 
* Represents certain data which other classes can retrieve
* through IInfoAssignable interface.
* To create custom Visual Info struct:
* -# Create a struct that inherits from {@code FVisualInfo}
* -# Add desired fields
* -# Override {@code Accept} function to implement Visitor pattern
* -# Override {@codeToString} function if needed
* 
* @seealso FVisualImageInfo
*/
USTRUCT(BlueprintInternalUseOnly)
struct FVisualInfo
{
	GENERATED_BODY()

public:
	FVisualInfo();

	virtual ~FVisualInfo();

	/**
	* Entry for visitor.
	*  
	* @see FVisualImageInfo::Accept()
	* 
	* @param Visitor interface object to assign this info to
	*/
	virtual void Accept(IInfoAssignable* Visitor) const PURE_VIRTUAL(FVisualInfo::Accept, );

	/**
	* @return String representation of this info
	*/
	virtual FString ToString() const PURE_VIRTUAL(FVisualInfo::ToString, return FString(););
};
