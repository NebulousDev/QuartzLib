#pragma once

#include "Types/Types.h"
#include <memory>

namespace Quartz
{
	/*====================================================
	|	               QUARTZLIB MEMORY                  |
	=====================================================*/

	// TODO: Implement later (with intrinsics)
	inline void* MemMove(void* pDest, void* pSource, uSize size)
	{
		return std::memmove(pDest, pSource, size);
	}

	// TODO: Implement later (with intrinsics)
	inline void* MemCopy(void* pDest, void* pSource, uSize size)
	{
		return std::memcpy(pDest, pSource, size);
	}
}