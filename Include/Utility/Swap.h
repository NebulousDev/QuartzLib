#pragma once

#include "Move.h"

namespace Quartz
{
	/*====================================================
	|                   QUARTZLIB SWAP                   |
	=====================================================*/

	template<typename Type>
	void Swap(Type& t1, Type& t2)
	{
		Type temp = Move(t1);
		t1 = Move(t2);
		t2 = Move(temp);
	}

	template<typename Type>
	void SwapNoMove(Type& t1, Type& t2)
	{
		Type temp = t1;
		t1 = t2;
		t2 = temp;
	}
}