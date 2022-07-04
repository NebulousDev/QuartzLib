#pragma once

#include "Utility/Template.h"
#include "Utility/Move.h"

namespace Quartz
{
	/*====================================================
	|                   QUARTZLIB TUPLE                  |
	=====================================================*/

	/** A single tuple value */
	template<sSize index, typename Type>
	class TupleValue
	{
	private:
		Type value;

	public:
		constexpr TupleValue() = default;

		constexpr TupleValue(const Type& value)
			: value(value) { }

		constexpr TupleValue(Type&& value)
			: value(Move(value)) { }

		constexpr Type& Value()
		{
			return value;
		}
	};

	/** Base class type for Tuple */
	template<sSize index, typename... Types>
	struct _TupleBase;

	/** Base case */
	template<sSize index>
	struct _TupleBase<index> {};

	template<sSize index, typename Type, typename... Types>
	struct _TupleBase<index, Type, Types...>
		: public TupleValue<index, StripReference<Type>>, public _TupleBase<index + 1, Types...>
	{
		constexpr _TupleBase() = default;

		template<typename ValType>
		constexpr _TupleBase(ValType&& value)
			: TupleValue<index, Type>(Forward<ValType>(value)) {}

		template<typename ValType, typename... ValTypes>
		constexpr _TupleBase(ValType&& value, ValTypes&&... values)
			: TupleValue<index, Type>(Forward<ValType>(value)),
			_TupleBase<index + 1, Types...>(Forward<ValTypes>(values)...) {}
	};

	////////////////////////////////////////////////////////////////////////

	/**
		Retrieves a Tuple element by type

		Note: This utility will return *only the first element of that type*,
		and will not work with tuples containing more than one of the same
		element type.
	*/
	template<sSize index, typename _Type, typename... Types>
	struct _TupleGetType
	{
		using Type = typename _TupleGetType<index - 1, Types...>::Type;
	};

	template<typename _Type, typename... Types>
	struct _TupleGetType<0, _Type, Types...>
	{
		using Type = _Type;
	};

	////////////////////////////////////////////////////////////////////////
	
	/* Retrieves a Tuple element by index */
	template<typename Search, sSize index, typename... Types>
	struct _TupleGetIndex;

	template<typename Search, sSize _index, typename Type, typename... Types>
	struct _TupleGetIndex<Search, _index, Type, Types...>
	{
		constexpr static sSize index = ConditionIndex<IsSameType<Search, Type>::value, _index, _TupleGetIndex<Search, _index + 1, Types...>::index>::index;
	};

	template<typename Search, sSize _index, typename Type>
	struct _TupleGetIndex<Search, _index, Type>
	{
		constexpr static sSize index = ConditionIndex<IsSameType<Search, Type>::value, _index, -1>::index;
	};

	////////////////////////////////////////////////////////////////////////

	/**
		A compile-time storage type for a tuple of elements
		
		Note: A Tuple must contain at least one type
	*/
	template<typename Type, typename... Types>
	struct Tuple : public _TupleBase<0, Type, Types...>
	{
		constexpr Tuple() = default;

		template<typename... ValTypes>
		constexpr Tuple(ValTypes&&... values)
			: _TupleBase<0, Type, Types...>(Forward<ValTypes>(values)...) {}

		template<sSize index>
		constexpr auto Get()
		{
			return static_cast<TupleValue<index, typename _TupleGetType<index, Type, Types...>::Type>>(*this).Value();
		}

		template<typename _Type>
		constexpr auto Get()
		{
			return static_cast<TupleValue<_TupleGetIndex<_Type, 0, Type, Types...>::index, _Type>>(*this).Value();
		}

		constexpr uSize Size() const
		{
			return sizeof...(Types) + 1;
		}
	};
}