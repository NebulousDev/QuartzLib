#pragma once

#include "Allocator.h"

namespace Quartz
{
	/*====================================================
	|	           QUARTZLIB POOL ALLOCATOR              |
	=====================================================*/

	template<typename ValueType, typename SizeType = uInt32>
	class PoolAllocator : public Allocator<PoolAllocator<ValueType, SizeType>, SizeType>
	{
	private:
		Allocator*	mpParent;
		SizeType	mMaxSizeBytes;
		SizeType	mMaxSize;
		SizeType	mSize;
		SizeType	mNextFreeIdx;
		ValueType*	mpData;

	private:
		constexpr static SizeType VALUE_SIZE =
			sizeof(ValueType) < sizeof(SizeType) ? sizeof(SizeType) : sizeof(ValueType);

	public:
		PoolAllocator(SizeType sizeBytes, Allocator* pParentAllocator = nullptr) :
			mpParent(pParentAllocator), mMaxSizeBytes(sizeBytes),
			mMaxSize(sizeBytes / VALUE_SIZE), mSize(0), mNextFreeIdx(0)
		{
			if (mpParent)
			{
				//mpData = pParentAllocator.Allocate(sizeBytes);
			}
			else
			{
				mpData = reinterpret_cast<ValueType*>(MemAlloc(sizeBytes));
			}
		}

		~PoolAllocator()
		{
			if (mpParent)
			{
				//pParentAllocator.Free(mpData);
			}
			else
			{
				MemFree(mpData);
			}
		}

		template<typename... CtorValues>
		ValueType* Allocate(CtorValues&&... values)
		{
			if (mSize > mMaxSize)
			{
				// @TODO: Assert
				return nullptr;
			}

			void* pNextMem = reinterpret_cast<uInt8*>(mpData) + (mNextFreeIdx * VALUE_SIZE);
			void* pSizeMem = reinterpret_cast<uInt8*>(mpData) + (mSize * VALUE_SIZE);

			if(pNextMem == pSizeMem)
			{
				mNextFreeIdx++;
			}
			else
			{
				mNextFreeIdx = *reinterpret_cast<SizeType*>(pNextMem);
			}

			ValueType* pValue = new (pNextMem) ValueType(Forward<CtorValues>(values)...);
			mSize++;

			return pValue;
		}

		void Free(ValueType* pValue)
		{
			if (pValue)
			{
				pValue->~ValueType();
				*reinterpret_cast<SizeType*>(pValue) = mNextFreeIdx;
				mNextFreeIdx = (pValue - mpData) / VALUE_SIZE;
				mSize--;
			}
		}
	};
}