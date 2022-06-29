#pragma once

#include "SparseSet.h"
#include "Utility/Move.h"
#include <cassert>

namespace Quartz
{
	/*====================================================
	|                 QUARTZLIB BLOCK SET                |
	=====================================================*/

	template<typename _ValueType, typename _HandleType = uInt32, typename _IntType = uInt32, uSize blockSize = 64>
	class BlockSet : public SparseSet<_HandleType, _IntType, blockSize>
	{
	public:
		using ValueType		= _ValueType;
		using HandleType	= _HandleType;
		using IntType		= _IntType;
		using SparseSet		= Quartz::SparseSet<_HandleType, _IntType, blockSize>;
		using Iterator		= typename Array<ValueType>::Iterator;
	
	private:
		Array<ValueType> mBlockSet;

	public:
		BlockSet()
			: SparseSet(), mBlockSet() {}

		template<typename Value>
		ValueType& Insert(HandleType handle, Value&& value)
		{
			IntType index = SparseSet::Insert(handle);

			if (index < mBlockSet.Size())
			{
				return mBlockSet[index] = Forward<Value>(value);
			}

			return mBlockSet.PushBack(Forward<Value>(value));
		}

		void Remove(HandleType handle)
		{
			assert(SparseSet::Contains(handle) && "BlockSet does not contain handle!");

			IntType index = SparseSet::Index(handle);
			mBlockSet[index] = mBlockSet[mBlockSet.Size() - 1];
			mBlockSet.PopBack();

			SparseSet::Remove(handle);
		}

		bool Contains(HandleType handle) const
		{
			return SparseSet::Contains(handle);
		}

		ValueType& Get(HandleType handle)
		{
			assert(SparseSet::Contains(handle) && "BlockSet does not contain handle!");
			return mBlockSet[SparseSet::Index(handle)];
		}

		Iterator begin()
		{
			return mBlockSet.begin();
		}

		Iterator end()
		{
			return mBlockSet.end();
		}

		uSize Size() const
		{
			return SparseSet::Size();
		}
	};
}