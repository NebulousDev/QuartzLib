#pragma once

#include "Types/Types.h"
#include <memory>
#include <cstring>

namespace Quartz
{
	/*====================================================
	|	               QUARTZLIB MEMORY                  |
	=====================================================*/

	// TODO: Implement later (with intrinsics)
	inline void* MemMove(void* pDest, void* pSource, uSize size)
	{
		return memmove(pDest, pSource, size);
	}

	// TODO: Implement later (with intrinsics)
	inline void* MemCopy(void* pDest, void* pSource, uSize size)
	{
		memcpy_s(pDest, size, pSource, size);
		return pDest;
	}
}