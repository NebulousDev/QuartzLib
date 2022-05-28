#pragma once

#include <initializer_list>

namespace Quartz
{
	/*====================================================
	|	         QUARTZLIB INITIALIZER LIST              |
	=====================================================*/

	// Note:
	// std::initializer_list is ingrained in the C++ standard and cannot be emulated.
	// This class is simply a wrapper for consistency.

	template<typename ValueType>
	using InitializerList = std::initializer_list<ValueType>;
}