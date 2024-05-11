#pragma once

#include "Memory/PoolAllocator.h"

namespace Quartz
{
	/*====================================================
	|                   QUARTZLIB POOL                   |
	=====================================================*/

	template<typename ValueType>
	class Pool
	{
	private:
		PoolAllocator<ValueType> mAllocator;

	public:
		Pool(uSize maxCount) :
			mAllocator(maxCount * sizeof(ValueType))
		{
			
		}
	};
}