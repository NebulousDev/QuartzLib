#pragma once

#include "Table.h"
#include "Utility/Hash.h"

namespace Quartz
{
	/*====================================================
	|	                QUARTZLIB SET                    |
	=====================================================*/

	template<typename ValueType, typename HashType = uSize>
	class Set
	{
	public:
		using TableType	= Table<ValueType, HashType>;

		using Iterator		= Quartz::Iterator<Set, ValueType>;
		using ConstIterator = Quartz::ConstIterator<Set, ValueType>;

	private:
		Table<ValueType> mTable;

	private:
		friend void Swap(Set& set1, Set& set2)
		{
			using Quartz::Swap;
			Swap(set1.mTable, set2.mTable);
		}

	public:
		Set()
			: mTable() {}

		Set(uSize size)
			: mTable(size) {}

		Set(const Set& set)
			: mTable(set.mTable) {}

		Set(Set&& set) noexcept
		{
			Swap(*this, set);
		}

		template<typename RValueType>
		ValueType& Add(RValueType&& value)
		{
			return mTable.Insert(Hash(value), Forward<RValueType>(value));
		}

		template<typename RValueType>
		void Remove(RValueType&& value)
		{
			mTable.Remove(Hash(value), Forward<RValueType>(value));
		}

		bool Contains(const ValueType& value)
		{
			return mTable.Contains(Hash(value), value);
		}

		Iterator Begin()
		{
			if (IsEmpty())
			{
				return nullptr;
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
				return nullptr;
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
			if (IsEmpty())
			{
				return nullptr;
			}

			return Iterator(&(mTable.Data() + Capacity())->AsKeyValue());
		}

		ConstIterator End() const
		{
			if (IsEmpty())
			{
				return nullptr;
			}

			return ConstIterator(&(mTable.Data() + Capacity())->AsKeyValue());
		}

		void Clear()
		{
			mTable.Clear();
		}

		bool Reserve(uSize size)
		{
			return mTable.Reserve(size);
		}

		void Shrink()
		{
			mTable.Shrink();
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

		Set& operator=(Set set)
		{
			Swap(*this, set);
			return *this;
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