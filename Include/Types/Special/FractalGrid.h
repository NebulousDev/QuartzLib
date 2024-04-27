#pragma once

#include "Types/Types.h"
#include "Types/Map.h"
#include <cmath>

namespace Quartz
{
	struct FractalPoint
	{
		uSize h, x, y;

		FractalPoint() :
			h(-1), x(0), y(0) {}

		FractalPoint(uSize h, uSize x, uSize y) :
			h(h), x(x), y(y) {}

		/* Return the parent's point */
		FractalPoint Parent() const
		{
			if (h % 2 == 0)
			{
				return FractalPoint(h + 1, floor((x + 1) / 2), floor((y + 1) / 2));
			}
			else
			{
				return FractalPoint(h + 1, floor(x / 2), floor(y / 2));
			}
		}

		/* Get a list of the child points, and return the size of the list */
		uSize Children(FractalPoint* pOutChildren) const
		{
			for (uSize cy = 0; cy < 2; cy++)
			{
				for (uSize cx = 0; cx < 2; cx++)
				{
					pOutChildren[cx + cy * 2] = FractalPoint(h + 1, 2 * x + cx, 2 * y + cy);
				}
			}

			return 4;
		}
	};

	template<typename CellValue>
	class FractalGrid
	{
	private:
		Map<FractalPoint, CellValue> mTileMap;

	private:
		void BuildParents(FractalPoint point)
		{
			auto& it = mTileMap.Find(point);

			if (it != mTileMap.End() || 
				point.x == 0 && point.y == 0)
			{
				return;
			}
			else
			{
				mTileMap.Get(point);
				BuildParents(point.Parent());
			}
		}

	public:
		CellValue& Place(const FractalPoint& point, const CellValue& value)
		{
			CellValue& cellValue = mTileMap.Put(point, value);
			BuildParents(point.Parent());
			return cellValue;
		}

		CellValue& Place(uSize h, uSize x, uSize y, CellValue& value)
		{
			return Place(FractalPoint( h, x, y ), value);
		}

		void Remove(const FractalPoint& point)
		{
			auto& it = mTileMap.Find(point);

			if (it != mTileMap.End())
			{
				mTileMap.Remove(it);

				if (point.h != 0)
				{
					FractalPoint children[4];
					point.Children(children);

					for (uSize i = 0; i < 4; i++)
					{
						Remove(children[i]);
					}
				}
			}
			else
			{
				return;
			}
		}

		void Remove(uSize h, uSize x, uSize y)
		{
			Remove(FractalPoint(h, x, y));
		}

		void Clear()
		{
			mTileMap.Clear();
		}

		CellValue* Get(const FractalPoint& point)
		{
			auto& it = mTileMap.Find(point);
			if (it != mTileMap.End())
			{
				return &it->value;
			}
			else
			{
				return nullptr;
			}
		}

		CellValue* Get(uSize h, uSize x, uSize y)
		{
			return Get(FractalPoint(h, x, y));
		}

		bool Has(const FractalPoint& point) const
		{
			return mTileMap.Find(point) != mTileMap.End();
		}
		
		bool Has(uSize h, uSize x, uSize y) const
		{
			return Has(FractalPoint(h, x, y));
		}

		uSize Size() const { return mTileMap.Size(); }
		uSize Top() const {}
	};

	template<>
	inline uSize Hash<FractalPoint>(const FractalPoint& value)
	{
		return Hash<uSize>(value.h * 35791113 ^ value.x * 24681012 ^ value.y * 346791013);
	}

	inline bool operator==(const FractalPoint& point0, const FractalPoint& point1)
	{
		return point0.h == point1.h && point0.x == point1.x && point0.y == point1.y;
	}
}