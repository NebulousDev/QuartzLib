#pragma once

#include "Memory.h"
#include "Types/Types.h"
#include "Utility/Move.h"

namespace Quartz
{
	/*====================================================
	|	             QUARTZLIB ALLOCATOR                 |
	=====================================================*/

	class Allocator {};

	template<typename AllocatorType, typename SizeType = uSize>
	class AllocatorBase : public Allocator
	{
	private:

	public:

	};
}