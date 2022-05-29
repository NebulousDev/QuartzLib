#pragma once

#include "Array.h"

namespace Quartz
{
	/*====================================================
	|	               QUARTZLIB STACK                   |
	=====================================================*/

	template<class ValueType>
	class ArrayStack : protected Array<ValueType>
	{
	public:
		using ArrayType = Array<ValueType>;

	protected:
		using Array<ValueType>::mpData;
		using Array<ValueType>::mSize;

	public:
		ArrayStack() {}

		ArrayStack(const ArrayStack& stack)
			: ArrayType(stack) {}

		ArrayStack(ArrayStack&& stack)
			: ArrayType(Move(stack)) {}

		ArrayStack(const ArrayType& array)
			: ArrayType(array) {}

		ArrayStack(ArrayType&& array)
			: ArrayType(Move(array)) {}

		template<typename RValueType>
		void Push(RValueType&& value)
		{
			PushBack(Forward<RValueType>(value));
		}

		ValueType* Peek()
		{
			if (mSize == 0)
			{
				return nullptr;
			}

			return &mpData[mSize - 1];
		}

		ValueType Pop()
		{
			if (mSize == 0)
			{
				// @Todo: throw error
				return ValueType();
			}

			ValueType value = Move(mpData[mSize - 1]);

			ArrayType::Remove(mSize - 1);

			return value;
		}

		using ArrayType::Contains;

		using ArrayType::Begin;
		using ArrayType::End;

		using ArrayType::Data;
		using ArrayType::Size;
		using ArrayType::Capacity;
		using ArrayType::IsEmpty;

		// for-each functions:

		using ArrayType::begin;
		using ArrayType::end;
	};

	template<class ValueType>
	using Stack = ArrayStack<ValueType>;
}