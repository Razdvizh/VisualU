// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Scenario.h"
#include "VisualVersioningSubsystem.generated.h"

class UDataTable;
class UVisualController;

//
//template<typename T, typename U>
//class HasMemberOfTypeImpl {
//private:
//
//	template<typename C>
//	static std::true_type test(decltype(static_cast<U C::*>(nullptr))*);
//
//	template<typename>
//	static std::false_type test(...);
//
//public:
//	static constexpr bool value = decltype(test<T>(nullptr))::value;
//};
//
//
//template<typename T, typename U>
//concept HasMemberOfType = HasMemberOfTypeImpl<T, U>::value;

/**
 * Allows for altering scenarios in chosen Data Tables.
 */
UCLASS()
class VISUALU_API UVisualVersioningSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	UVisualVersioningSubsystem();

	/*
	* Chooses a provided version of Data Table in Visual Controller
	* Returns result of choosing
	*/
	UFUNCTION(Category = "Visual Versioning")
	bool ChooseVersion(UVisualController* VisualController, const UDataTable* DataTable, int32 Index, const FVisualScenarioInfo& Version);

	/*
	* Switches subsystem to an older version
	*/
	void Checkout(FScenario* const Scene) const;

	//template<typename T, typename... V>
	//inline void Test(T* Obj, V T::*... Pointers, const V&... Values)
	//{
	//	((Obj->*Pointers = Values), ...);
	//}
	
	virtual void Deinitialize() override;
	
private:
	TMultiMap<FScenario*, FVisualScenarioInfo> Versions;

};
