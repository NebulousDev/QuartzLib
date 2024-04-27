#pragma once

#include "../Types.h"

namespace Quartz
{
	template<typename ValueType, uSize MAX_DEPTH>
	class QuadTree
	{
	private:
		struct QuadTreeTile
		{
			float x, y;
			float width;
			float height;

			QuadTreeTile* pParent;
			QuadTreeTile* pChildren[2][2];
		};

	private:

	public:


	};
}