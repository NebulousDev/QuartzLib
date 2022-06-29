#pragma once

#include "Types/Types.h"

namespace Quartz
{
	/*====================================================
	|            QUARTZLIB TEMPLATE UTILITIES            |
	=====================================================*/

	//////////////////////////////////////////////////////////////

	/** A constant value */
	template<typename _ValueType, _ValueType val>
	struct CompileConstant
	{
		using ValueType = _ValueType;
		constexpr static ValueType value = val;
		constexpr operator ValueType() const noexcept { return value; }
	};

	//////////////////////////////////////////////////////////////

	using TrueType = CompileConstant<bool, true>;
	using FalseType = CompileConstant<bool, false>;

	//////////////////////////////////////////////////////////////

	/* A list of types */
	template<typename... Types>
	struct TypeList;

	template<typename... Types>
	struct TypeListSize : public CompileConstant<uSize, sizeof...(Types)> {};

	template<typename... Types>
	struct TypeList
	{
		constexpr static uSize size = TypeListSize<Types...>::value;
	};

	//////////////////////////////////////////////////////////////

	/* Checks if two types are equal */
	template<typename Type1, typename Type2>
	struct IsSameType : public FalseType {};

	template<typename Type>
	struct IsSameType<Type, Type> : public TrueType {};

	//////////////////////////////////////////////////////////////

	template<bool condition, typename T = void>
	struct _EnableIf {};

	template<typename T>
	struct _EnableIf<true, T> { typedef T Type; };

	//////////////////////////////////////////////////////////////

	/*
		Applies a conditional statement with types.
		If value is true, Type will be the first type,
		if value is false, Type will be the second type
	*/
	template<bool condition, typename TrueType, typename FalseType>
	struct Condition
	{
		using Type = TrueType;
	};

	template<typename TrueType, typename FalseType>
	struct Condition<false, TrueType, FalseType>
	{
		using Type = FalseType;
	};

	//////////////////////////////////////////////////////////////

	/*
		Applies a conditional statement with indices.
		If value is true, index will be the first value,
		if value is false, index will be the second value
	*/
	template<bool value, uSize trueValue, uSize falseValue>
	struct ConditionIndex
	{
		constexpr static uSize index = trueValue;
	};

	template<uSize trueValue, uSize falseValue>
	struct ConditionIndex<false, trueValue, falseValue>
	{
		constexpr static uSize index = falseValue;
	};
}