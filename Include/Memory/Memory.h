#pragma once

#include "Types/Types.h"
#include <memory>
#include <cstring>

namespace Quartz
{
	/*====================================================
	|	               QUARTZLIB MEMORY                  |
	=====================================================*/

	#define KIBIBYTE 1024UL
	#define MEBIBYTE KIBIBYTE * 1024UL
	#define GIBIBYTE MEBIBYTE * 1024UL
	
	#define KILOBYTE 1000UL
	#define MEGABYTE KILOBYTE * 1000UL
	#define GIGABYTE MEGABYTE * 1000UL

	inline void* MemAlloc(uSize sizeBytes)
	{
		return malloc(sizeBytes);
	}

	inline void MemFree(void* pMemory)
	{
		free(pMemory);
	}

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