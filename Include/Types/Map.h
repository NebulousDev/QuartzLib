#pragma once

#include "Table.h"
#include "Utility/Hash.h"

namespace Quartz
{
	/*====================================================
	|                   QUARTZLIB MAP                    |
	=====================================================*/

	template<typename KeyType, typename ValueType>
	struct MapPair
	{
		KeyType		key;
		ValueType	value;

		MapPair()
			: key(), value() {}

		MapPair(const KeyType& key)
			: key(key), value() { }

		MapPair(KeyType&& key)
			: key(Move(key)), value() { }

		MapPair(const KeyType& key, const ValueType& value)
			: key(key), value(value) { }

		MapPair(KeyType&& key, ValueType&& value)
			: key(Move(key)), value(Move(value)) { }

		MapPair(const MapPair& pair)
			: key(pair.key), value(pair.value) {}

		MapPair(MapPair&& pair) noexcept
		{
			Swap(*this, pair);
		}

		friend void Swap(MapPair& pair1, MapPair& pair2)
		{
			using Quartz::Swap;
			Swap(pair1.key, pair2.key);
			Swap(pair1.value, pair2.value);
		}

		MapPair& operator=(MapPair pair)
		{
			Swap(*this, pair);
			return *this;
		}

		bool operator==(const MapPair& pair) const
		{
			return key == pair.key;
		}

		bool operator!=(const MapPair& pair) const
		{
			return key != pair.key;
		}
	};

	template<typename KeyType, typename ValueType, typename HashType = uSize>
	class Map
	{
	public:
		using PairType	= MapPair<KeyType, ValueType>;
		using TableType = Table<PairType, HashType>;

		using Iterator		= Quartz::Iterator<Map, PairType>;
		using ConstIterator = Quartz::ConstIterator<Map, PairType>;

	private:
		TableType mTable;

	private:
		friend void Swap(Map& map1, Map& map2)
		{
			using Quartz::Swap;
			Swap(map1.mTable, map2.mTable);
		}

	public:
		Map()
			: mTable() {}

		Map(uSize size)
			: mTable(size) {}

		Map(const Map& map)
			: mTable(map.mTable) {}

		Map(Map&& map) noexcept
		{
			Swap(*this, map);
		}

		template<typename RKeyType, typename RValueType>
		ValueType& Put(RKeyType&& key, RValueType&& value)
		{
			uSize hash = Hash(key);
			return mTable.Insert(hash, PairType(Forward<RKeyType>(key), 
				Forward<RValueType>(value))).value;
		}

		template<typename RKeyType>
		ValueType& Put(RKeyType&& key)
		{
			uSize hash = Hash(key);
			return mTable.Insert(hash, PairType(Forward<RKeyType>(key), ValueType())).value;
		}

		template<typename RKeyType>
		ValueType& Get(RKeyType&& key)
		{
			PairType& pair = mTable.FindInsert(Hash(key), PairType(Forward<RKeyType>(key)));
			return pair.value;
		}

		template<typename RKeyType>
		void Remove(RKeyType&& key)
		{
			mTable.Remove(Hash(key), PairType(Forward<RKeyType>(key)));
		}

		template<typename RKeyType>
		Iterator Find(RKeyType&& key)
		{
			typename TableType::Iterator tableIt = 
				mTable.Find(Hash(key), PairType(Forward<RKeyType>(key)));

			return Iterator(&tableIt->AsKeyValue());
		}

		template<typename RKeyType>
		ConstIterator Find(RKeyType&& key) const
		{
			typename TableType::ConstIterator tableIt = 
				mTable.Find(Hash(key), PairType(Forward<RKeyType>(key)));

			return ConstIterator(&tableIt->AsKeyValue());
		}

		template<typename RKeyType>
		bool Contains(RKeyType&& key)
		{
			return mTable.Contains(Hash(key), PairType(Forward<RKeyType>(key)));
		}

		Iterator Begin()
		{
			if (IsEmpty())
			{
				return End();
			}

			Iterator it(&mTable.Data()->AsKeyValue());

			if (mTable.Data()->IsEmpty())
			{
				return ++it;
			}

			return it;
		}

		ConstIterator Begin() const
		{
			if (IsEmpty())
			{
				return End();
			}

			Iterator it(&mTable.Data()->AsKeyValue());

			if (mTable.Data()->IsEmpty())
			{
				return ++it;
			}

			return it;
		}

		Iterator End()
		{
			return Iterator(&(mTable.Data() + Capacity())->AsKeyValue());
		}

		ConstIterator End() const
		{
			return ConstIterator(&(mTable.Data() + Capacity())->AsKeyValue());
		}

		bool Reserve(uSize size)
		{
			return mTable.Reserve(size);
		}

		void Remove(const KeyType& key)
		{
			mTable.Remove(Hash(key), key);
		}

		void Shrink()
		{
			mTable.Shrink();
		}

		void Clear()
		{
			mTable.Clear();
		}

		uSize Size() const
		{
			return mTable.Size();
		}

		uSize Capacity() const
		{
			return mTable.Capacity();
		}

		uSize Threshold() const
		{
			return mTable.Threshold();
		}

		bool IsEmpty() const
		{
			return mTable.IsEmpty();
		}

		Map& operator=(Map map)
		{
			Swap(*this, map);
			return *this;
		}

		template<typename RKeyType>
		ValueType& operator[](RKeyType&& key)
		{
			return Get(Forward<RKeyType>(key));
		}

		// for-each functions:

		Iterator begin()
		{
			return Begin();
		}

		Iterator end()
		{
			return End();
		}

		ConstIterator begin() const
		{
			return Begin();
		}

		ConstIterator end() const
		{
			return End();
		}

		// Iterator overloads:

		friend Iterator& operator++(Iterator& itr)
		{
			typename TableType::Iterator& tableItr =
				reinterpret_cast<typename TableType::Iterator&>(itr);

			do
			{
				if (tableItr.pItr->IsLast())
				{
					++tableItr.pItr;
					return itr;
				}

				++tableItr.pItr;
			} while (tableItr.pItr->IsEmpty());

			return itr;
		}

		friend Iterator operator++(Iterator& itr, int)
		{
			Iterator temp(itr);
			++itr;
			return temp;
		}
	};
}