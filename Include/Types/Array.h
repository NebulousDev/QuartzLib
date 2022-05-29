#pragma once

#include "Types.h"
#include "Memory/Memory.h"
#include "Utility/Swap.h"
#include "Utility/Iterator.h"
#include "Utility/InitializerList.h"

namespace Quartz
{
	/*====================================================
	|	               QUARTZLIB ARRAY                   |
	=====================================================*/

	template<typename ValueType, typename SizeType = uSize>
	class Array
	{
	public:
		using Iterator		= Quartz::Iterator<Array, ValueType>;
		using ConstIterator = Quartz::ConstIterator<Array, ValueType>;

		constexpr static uSize RESIZE_FACTOR	= 1.5f;
		constexpr static uSize INITAL_SIZE		= 16;

	protected:
		ValueType*	mpData;
		SizeType	mSize;
		SizeType	mCapacity;

	protected:
		SizeType NextSize(SizeType size)
		{
			return size == 0 ? INITAL_SIZE : 
				static_cast<SizeType>((static_cast<float>(size) * RESIZE_FACTOR) + 0.5f);
		}

		void ReserveImpl(SizeType capacity, SizeType offset = 0)
		{
			ValueType* mpPrev = mpData;
			mpData = new ValueType[capacity * sizeof(ValueType)];

			for (SizeType i = 0; i < mSize; i++)
			{
				Swap(mpData[i + offset], mpPrev[i]);
			}

			mCapacity = capacity;

			delete[] mpPrev;
		}

	private:
		friend void Swap(Array& array1, Array& array2)
		{
			using Quartz::Swap;
			Swap(array1.mpData, array2.mpData);
			Swap(array1.mSize, array2.mSize);
			Swap(array1.mCapacity, array2.mCapacity);
		}

	public:
		Array()
			: mpData(nullptr), mSize(0), mCapacity(0) {}

		Array(SizeType size)
			: mSize(size), mCapacity(size)
		{
			mpData = new ValueType[size * sizeof(ValueType)];

			for (SizeType i = 0; i < mSize; i++)
			{
				// Construct new default-constructed value
				new (&mpData[i]) ValueType();
			}
		}

		Array(SizeType size, const ValueType& value)
			: mSize(size), mCapacity(size)
		{
			mpData = new ValueType[size * sizeof(ValueType)];

			for (SizeType i = 0; i < mSize; i++)
			{
				// Construct new value with given initial value
				new (&mpData[i]) ValueType(value);
			}
		}

		Array(const Array& array)
			: mSize(array.mSize), mCapacity(array.mCapacity)
		{
			mpData = new ValueType[array.mCapacity * sizeof(ValueType)];

			for (SizeType i = 0; i < mSize; i++)
			{
				// Construct value from given value from the array
				new (&mpData[i]) ValueType(array.mpData[i]);
			}
		}

		Array(Array&& array) noexcept
			: Array()
		{
			Swap(*this, array);
		}

		Array(InitializerList<ValueType> list)
			: Array(list.size())
		{
			for (SizeType i = 0; i < mSize; i++)
			{
				// Construct value from given value from the array
				new (&mpData[i]) ValueType(*(list.begin() + i));
			}
		}

		~Array()
		{
			delete[] mpData;
		}

		template<typename RValueType>
		ValueType& PushFront(RValueType&& value)
		{
			if (mSize + 1 > mCapacity)
			{
				// Reserve with offset of 1
				// so that we dont need to move
				// data again after resizing
				ReserveImpl(NextSize(mCapacity), 1);
			}
			else
			{
				// Move all values right by one
				MemMove(&mpData[1], &mpData[0], mSize * sizeof(ValueType));
			}

			++mSize;

			// Construct at the beginning of the array
			return *new (&mpData[0]) ValueType(Forward<RValueType>(value));

		}

		template<typename RValueType>
		ValueType& PushBack(RValueType&& value)
		{
			if (mSize + 1 > mCapacity)
			{
				ReserveImpl(NextSize(mCapacity));
			}

			// Construct at the end of the array
			return *new (&mpData[mSize++]) ValueType(Forward<RValueType>(value));
		}

		void Remove(SizeType index)
		{
			mpData[index].~ValueType();

			if (mSize > 0 && index < mSize)
			{
				// Move all values left by one
				MemMove(&mpData[index], &mpData[index + 1], (mSize - index) * sizeof(ValueType));
				--mSize;
			}
		}

		ValueType PopBack()
		{
			if (mSize > 0)
			{
				ValueType value = mpData[mSize - 1];
				Remove(mSize - 1);
				return value;
			}

			// @Todo: throw error
			return ValueType();
		}

		ValueType PopFront()
		{
			if (mSize > 0)
			{
				ValueType value = mpData[0];
				Remove(0);
				return value;
			}

			// @Todo: throw error
			return ValueType();
		}

		void Resize(SizeType size)
		{
			if (size < mSize)
			{
				// Cannot resize smaller than mSize
				return;
			}

			if (size > mCapacity)
			{
				// Size is larger than capacity, reserve more space
				ReserveImpl(size);
			}

			for (SizeType i = mSize; i < size; i++)
			{
				// Construct new default-constructed value
				new (&mpData[i]) ValueType();
			}

			mSize = size;
		}

		void Resize(SizeType size, const ValueType& value)
		{
			if (size > mSize)
			{
				if (size > mCapacity)
				{
					Reserve(NextSize(size));
				}

				for (SizeType i = mSize; i < size; i++)
				{
					// Construct new value with given initial value
					new (&mpData[i]) ValueType(value);
				}
			}

			else if (size < mSize)
			{
				for (SizeType i = mSize; i >= size; --i)
				{
					// Destruct valid entries
					mpData[i].~ValueType();
				}
			}

			mSize = size;
		}

		void Reserve(SizeType capacity)
		{
			if (capacity < mSize)
			{
				// Cannot resize smaller than mSize
				return;
			}

			ReserveImpl(capacity);
		}

		Iterator Find(const ValueType& value)
		{
			for (Iterator it = Begin(); it < End(); it++)
			{
				if (*it == value)
				{
					return it;
				}
			}

			return Iterator(End());
		}

		ConstIterator Find(const ValueType& value) const
		{
			for (ConstIterator it = Begin(); it < End(); it++)
			{
				if (*it == value)
				{
					return it;
				}
			}

			return ConstIterator(End());
		}

		Iterator Begin()
		{
			return Iterator(mpData);
		}

		ConstIterator Begin() const
		{
			return Iterator(mpData);
		}

		Iterator End()
		{
			return Iterator(mpData + mSize);
		}

		ConstIterator End() const
		{
			return Iterator(mpData + mSize);
		}

		bool Contains(const ValueType& value) const
		{
			for (const ValueType& match : *this)
			{
				if (match == value)
				{
					return true;
				}
			}

			return false;
		}

		void Shrink()
		{
			ReserveImpl(mSize);
		}

		void Clear()
		{
			delete[] mpData;
			mpData = new ValueType[INITAL_SIZE];

			mSize = 0;
			mCapacity = INITAL_SIZE;
		}

		ValueType* Data()
		{
			return mpData;
		}

		const ValueType* Data() const
		{
			return mpData;
		}

		SizeType Size() const
		{
			return mSize;
		}

		SizeType Capacity() const
		{
			return mCapacity;
		}

		bool IsEmpty() const
		{
			return mSize == 0;
		}

		Array& operator=(Array array)
		{
			Swap(*this, array);
			return *this;
		}

		ValueType& operator[](SizeType index)
		{
			return mpData[index];
		}

		const ValueType& operator[](SizeType index) const
		{
			return mpData[index];
		}

		// for-each functions:

		Iterator begin()
		{
			return Begin();
		}

		const ConstIterator begin() const
		{
			return Begin();
		}

		Iterator end()
		{
			return End();
		}

		const ConstIterator end() const
		{
			return End();
		}

		// Iterator overloads:

		friend Iterator& operator++(Iterator& itr)
		{
			++itr.pItr;
			return itr;
		}

		friend Iterator operator++(Iterator& itr, int)
		{
			Iterator temp(itr);
			++itr.pItr;
			return temp;
		}
	};
}