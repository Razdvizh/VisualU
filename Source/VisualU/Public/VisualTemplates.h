// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/*
* Checks if T has a member variable of type U
*/
template<typename T, typename U>
class VISUALU_API HasMemberOfTypeImpl {
private:

	template<typename C>
	static std::true_type test(decltype(static_cast<U C::*>(nullptr))*);

	template<typename>
	static std::false_type test(...);

public:
	static constexpr bool value = decltype(test<T>(nullptr))::value;
};

/*
* Checks if T has a member variable of type U
*/
template<typename T, typename U>
concept HasMemberOfType = HasMemberOfTypeImpl<T, U>::value;

/*
* Assignes values to the member variables of the object.
*/
template<typename T, typename... V>
VISUALU_API inline void UpdateMembers(T* Obj, V T::*... Pointers, const V&... Values)
{
	((Obj->*Pointers = Values), ...);
}
