#pragma once

#include "Types.h"
#include "Memory/Memory.h"
#include "Utility/Swap.h"
#include "Utility/Iterator.h"
#include "Utility/InitializerList.h"

#include <assert.h>

namespace Quartz
{
	/*====================================================
	|                  QUARTZLIB ARRAY                   |
	=====================================================*/

	template<typename ValueType, uSize SMALL_SIZE>
	class _SmallArray
	{
	protected:
		ValueType mSmall[SMALL_SIZE];
	};

	template<typename ValueType>
	class _SmallArray<ValueType, 0> {};

	template<typename ValueType, uSize SMALL_SIZE = 0>
	class Array : public _SmallArray<ValueType, SMALL_SIZE>
	{
	public:
		using SizeType		= uSize;
		using Iterator		= Quartz::Iterator<Array, ValueType>;
		using ConstIterator = Quartz::ConstIterator<Array, ValueType>;

		constexpr static bool IS_SMALL = SMALL_SIZE != 0;

		constexpr static float RESIZE_FACTOR	= 1.5f;
		constexpr static uSize INITAL_SIZE		= IS_SMALL ? SMALL_SIZE : 16;

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
			if constexpr (IS_SMALL)
			{
				assert(false && "Cannot reserve a 'small' Array (SMALL_SIZE > 0).");
			}

			ValueType* mpPrev = mpData;

			mpData = new ValueType[capacity];

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

			if constexpr (IS_SMALL)
			{
				uSize maxSize = array1.Size() > array2.Size() 
					? array1.Size() : array2.Size();

				for (uSize i = 0; i < maxSize; i++)
				{
					Swap(array1.mSmall[i], array2.mSmall[i]);
				}
			}
			else
			{
				Swap(array1.mpData, array2.mpData);
			}

			Swap(array1.mSize, array2.mSize);
			Swap(array1.mCapacity, array2.mCapacity);
		}

	public:
		Array()
			: mpData(nullptr), mSize(0), mCapacity(0)
		{
			if constexpr (IS_SMALL)
			{
				mpData = mSmall;
				mCapacity = SMALL_SIZE;
			}
		}

		Array(SizeType size)
			: mSize(size), mCapacity(size)
		{
			if constexpr (IS_SMALL)
			{
				if (size > SMALL_SIZE)
				{
					assert(false && "Cannot construct a 'small' Array (SMALL_SIZE > 0) with a size greater than SMALL_SIZE.");
				}

				mpData = mSmall;
				mCapacity = SMALL_SIZE;
			}
			else
			{
				mpData = new ValueType[size];
			}
		}

		Array(SizeType size, const ValueType& value)
			: Array(size)
		{
			for (SizeType i = 0; i < mSize; i++)
			{
				mpData[i] = value;
			}
		}

		template<uSize CTOR_SMALL_SIZE>
		Array(const Array<ValueType, CTOR_SMALL_SIZE>& array) :
			mSize(array.Size()), mCapacity(array.Capacity())
		{
			if constexpr (IS_SMALL)
			{
				assert(array.Size() >= SMALL_SIZE && "A 'small' Array (SMALL_SIZE > 0) can only be coppied to a 'large'\
 Array (SMALL_SIZE == 0) , or a 'small' Array of a greater or equal size.");

				mpData = mSmall;
				mCapacity = SMALL_SIZE;
			}
			else
			{
				mpData = new ValueType[mCapacity];
			}

			for (SizeType i = 0; i < mSize; i++)
			{
				mpData[i] = array[i];
			}
		}

		Array(const Array& array)
			: mSize(array.Size()), mCapacity(array.Capacity())
		{
			if constexpr (IS_SMALL)
			{
				assert(array.Size() <= SMALL_SIZE && "A 'small' Array (SMALL_SIZE > 0) can only be coppied to a 'large'\
 Array (SMALL_SIZE == 0), or a 'small' Array of a greater or equal size.");
		
				mpData = mSmall;
				mCapacity = SMALL_SIZE;
			}
			else
			{
				mpData = new ValueType[mCapacity];
			}
		
			for (SizeType i = 0; i < mSize; i++)
			{
				mpData[i] = array[i];
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
				mpData[i] = *(list.begin() + i);
			}
		}

		~Array()
		{
			if constexpr (!IS_SMALL)
			{
				delete[] mpData;
			}
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
			mpData[0] = Forward<RValueType>(value);

			return mpData[0];
		}

		template<typename RValueType>
		ValueType& PushBack(RValueType&& value)
		{
			if (mSize + 1 > mCapacity)
			{
				ReserveImpl(NextSize(mCapacity));
			}

			// Construct at the end of the array
			mpData[mSize] = Forward<RValueType>(value);

			return mpData[mSize++];
		}

		void Remove(SizeType index) 
		{
			assert(index < mSize && "Array index out of bounds.");

			if (index < mSize)
			{
				mpData[index].~ValueType();

				// Move all values left by one
				MemMove(&mpData[index], &mpData[index + 1], (mSize - index) * sizeof(ValueType));
				--mSize;

				new (&mpData[mSize]) ValueType();
			}
		}

		void Remove(const ValueType& value)
		{
			Iterator& itr = Find(value);
			uSize index = itr.pItr - mpData;
			Remove(index);
		}

		void Remove(const Iterator& itr)
		{
			uSize index = itr.pItr - mpData;
			Remove(index);
		}

		// FastRemove will swap the removed element with the last in the list
		// This function does NOT preserve insertion order
		void FastRemove(SizeType index)
		{
			assert(index < mSize && "Array index out of bounds.");

			Swap(mpData[index], mpData[mSize]);
			mpData[mSize].~ValueType();
			mSize--;
		}

		// FastRemove will swap the removed element with the last in the list
		// This function does NOT preserve insertion order
		void FastRemove(const ValueType& value)
		{
			Iterator& itr = Find(value);
			uSize index = itr.pItr - mpData;
			FastRemove(index);
		}

		// FastRemove will swap the removed element with the last in the list
		// This function does NOT preserve insertion order
		void FastRemove(const Iterator& itr)
		{
			uSize index = itr.pItr - mpData;
			FastRemove(index);
		}

		ValueType PopBack()
		{
			assert(mSize > 0 && "Cannot pop from an empty Array.");

			if (mSize > 0)
			{
				ValueType value = mpData[mSize - 1];
				Remove(mSize - 1);
				return value;
			}

			return ValueType();
		}

		ValueType PopFront()
		{
			assert(mSize > 0 && "Cannot pop from an empty Array.");

			if (mSize > 0)
			{
				ValueType value = mpData[0];
				Remove(0);
				return value;
			}

			return ValueType();
		}

		void Resize(SizeType size)
		{
			assert(size > mSize && "Cannot resize Array less than the current size.");

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
			assert(size > mSize && "Cannot resize Array less than the current size.");

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
				for (SizeType i = mSize - 1; i >= size; --i)
				{
					// Destruct valid entries
					mpData[i].~ValueType();
				}
			}

			mSize = size;
		}

		void Reserve(SizeType capacity)
		{
			assert(capacity > mSize && "Cannot reserve Array less than the current size.");

			if (capacity < mSize)
			{
				// Cannot resize smaller than mSize
				return;
			}

			ReserveImpl(capacity);
		}

		uSize IndexOf(const ValueType& value) const
		{
			for (ValueType* pValue = mpData; pValue < mpData + mCapacity; pValue++)
			{
				if (*pValue == value)
				{
					return pValue - mpData;
				}
			}

			return (uSize)-1;
		}

		uSize IndexOf(const Iterator& itr) const
		{
			return IndexOf(*itr.pItr);
		}

		uSize IndexOf(const ConstIterator& itr) const
		{
			return IndexOf(*itr.pItr);
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

		Iterator begin()
		{
			return Iterator(mpData);
		}

		ConstIterator begin() const
		{
			return ConstIterator(mpData);
		}

		Iterator end()
		{
			return Iterator(mpData + mSize);
		}

		ConstIterator end() const
		{
			return ConstIterator(mpData + mSize);
		}

		Iterator rbegin()
		{
			return Iterator(mpData + mSize - 1);
		}

		ConstIterator rbegin() const
		{
			return ConstIterator(mpData + mSize - 1);
		}

		Iterator rend()
		{
			return Iterator(mpData - 1);
		}

		ConstIterator rend() const
		{
			return ConstIterator(mpData - 1);
		}

		/// Legacy Begin/End ///

		Iterator Begin()
		{
			return begin();
		}

		const ConstIterator Begin() const
		{
			return begin();
		}

		Iterator End()
		{
			return end();
		}

		const ConstIterator End() const
		{
			return end();
		}

		////////////////////////

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
			for (uSize i = 0; i < mSize; i++)
			{
				(&mpData[i])->~ValueType();
			}

			mSize = 0;
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

		bool IsSmall() const
		{
			return IS_SMALL;
		}

		Array& operator=(Array array)
		{
			Swap(*this, array);
			return *this;
		}

		ValueType& operator[](SizeType index)
		{
			assert(index < mSize && "Array index out of bounds.");

			return mpData[index];
		}

		const ValueType& operator[](SizeType index) const
		{
			assert(index < mSize && "Array index out of bounds.");

			return mpData[index];
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