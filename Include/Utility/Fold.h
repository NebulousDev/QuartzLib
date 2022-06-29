#pragma once

namespace Quartz
{
	/*====================================================
	|                   QUARTZLIB FOLD                   |
	=====================================================*/

	/*
		Applys a compare and fold left for each value
		Note: FoldCompare requires at least one type.
	*/
	template<typename CompareFunc, typename Type, typename... Types>
	Type FoldCompare(CompareFunc comp, Type value, Types... values)
	{
		if constexpr (sizeof...(values) == 0)
		{
			return value;
		}
		else
		{
			Type recurValue = FoldCompare<CompareFunc, Types...>(comp, values...);
			return comp(value, recurValue) ? value : recurValue;
		}
	}
}