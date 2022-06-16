#pragma once

#include "Types.h"
#include "Array.h"
#include "Utility/Swap.h"
#include "Utility/Iterator.h"

namespace Quartz
{
	/*====================================================
	|                  QUARTZLIB TABLE                   |
	=====================================================*/

	template<typename KeyValueType, typename HashType>
	class TableEntry
	{
	public:
		KeyValueType	keyValue;
		HashType		hash;
		uInt16			probe;
		bool			used;

		union
		{
			struct
			{
				bool first		: 1;
				bool last		: 1;
				bool reserved	: 6;
			};

			uInt8 meta;
		};

	public:
		TableEntry()
			: keyValue(), hash(0), probe(0), meta(0), used(false) { }

		TableEntry(HashType hash, const KeyValueType& keyValue)
			: hash(hash), keyValue(keyValue), probe(0), meta(0), used(true) {}

		TableEntry(HashType hash, KeyValueType&& keyValue)
			: hash(hash), keyValue(Move(keyValue)), probe(0), meta(0), used(true) {}

		TableEntry(const TableEntry& entry)
			: hash(entry.hash), keyValue(entry.keyValue), probe(0), used(entry.used), meta(0) {}

		TableEntry(TableEntry&& entry) noexcept
			: TableEntry()
		{
			Swap(*this, entry);
		}

		TableEntry& operator=(TableEntry entry)
		{
			Swap(*this, entry);
			return *this;
		}

		friend void Swap(TableEntry& entry1, TableEntry& entry2)
		{
			using Quartz::Swap;
			Swap(entry1.hash, entry2.hash);
			Swap(entry1.keyValue, entry2.keyValue);
			Swap(entry1.probe, entry2.probe);
			Swap(entry1.used, entry2.used);
		}

		bool IsEmpty() const
		{
			return !used;
		}

		bool IsFirst() const
		{
			return first;
		}

		bool IsLast() const
		{
			return last;
		}

		KeyValueType& AsKeyValue()
		{
			return reinterpret_cast<KeyValueType&>(*this);
		}
	};

	template<typename KeyValueType, typename HashType = uSize>
	class Table
	{
	public:
		using TableType = Table<KeyValueType, HashType>;
		using EntryType = TableEntry<KeyValueType, HashType>;

		using Iterator		= Quartz::Iterator<TableType, EntryType>;
		using ConstIterator = Quartz::ConstIterator<TableType, EntryType>;

		constexpr static float	LOAD_FACTOR = 0.85f;
		constexpr static uSize	INITAL_SIZE = 16;

	private:
		friend void Swap(Table& table1, Table& table2)
		{
			using Quartz::Swap;
			Swap(table1.mTable, table2.mTable);
			Swap(table1.mSize, table2.mSize);
			Swap(table1.mCapacity, table2.mCapacity);
			Swap(table1.mThreshold, table2.mThreshold);
		}

	private:
		Array<EntryType> mTable;
		HashType mSize;
		HashType mCapacity;
		HashType mThreshold;

		inline uSize WrapIndex(uSize index) const
		{
			return index & (mCapacity - 1);
		}

		inline uSize GetIndex(HashType hash) const
		{
			return hash & (mCapacity - 1);
		}

		inline HashType GetDistance(HashType hash, HashType index) const
		{
			return GetIndex(hash) - index;
		}

		EntryType* FindEntry(HashType hash, const KeyValueType& keyValue)
		{
			HashType index	= GetIndex(hash);
			HashType dist	= 0;

			while (true)
			{
				const EntryType& entry = mTable[index];

				if (entry.IsEmpty() || dist > entry.probe)
				{
					return nullptr;
				}

				if (entry.hash == hash && entry.keyValue == keyValue)
				{
					return &mTable[index];
				}

				index = WrapIndex(index + 1);
				++dist;
			}

			return nullptr;
		}

		const EntryType* FindEntry(HashType hash, const KeyValueType& keyValue) const
		{
			HashType index	= GetIndex(hash);
			HashType dist	= 0;

			while (true)
			{
				const EntryType& entry = mTable[index];

				if (entry.IsEmpty() || dist > entry.probe)
				{
					return nullptr;
				}

				if (entry.hash == hash && entry.keyValue == keyValue)
				{
					return &mTable[index];
				}

				index = WrapIndex(index + 1);
				++dist;
			}

			return nullptr;
		}

		template<typename RKeyValueType>
		EntryType& FindInsertEntry(HashType hash, RKeyValueType&& keyValue)
		{
			EntryType* pEntry = FindEntry(hash, keyValue);

			if (!pEntry)
			{
				if (mSize + 1 >= mThreshold)
				{
					ResizeRehash(NextGreaterPowerOf2(mCapacity));
				}

				return InsertImpl(hash, Forward<RKeyValueType>(keyValue));
			}

			return *pEntry;
		}

		template<typename RKeyValueType>
		EntryType& InsertImpl(HashType hash, RKeyValueType&& keyValue)
		{
			HashType index = GetIndex(hash);

			EntryType entry(hash, Forward<RKeyValueType>(keyValue));

			while (true)
			{
				EntryType& current = mTable[index];

				// Entry is empty
				if (current.IsEmpty())
				{
					++mSize;
					Swap(entry, current);
					return current;
				}

				// Found previous key entry
				if (current.keyValue == entry.keyValue)
				{
					Swap(entry, current);
					return current;
				}

				// Robin hood
				if (current.probe < entry.probe)
				{
					Swap(entry, current);
				}

				++entry.probe;
				index = WrapIndex(index + 1);
			}
		}

		void ResizeRehash(HashType size)
		{
			Table mNewTable(size);

			EntryType* pEntry = &mTable[0];

			while (!pEntry->IsLast())
			{
				if (!pEntry->IsEmpty())
				{
					mNewTable.Insert(pEntry->hash, Move(pEntry->keyValue));
				}

				++pEntry;
			}

			if (!pEntry->IsEmpty())
			{
				mNewTable.Insert(pEntry->hash, Move(pEntry->keyValue));
			}

			Swap(mNewTable, *this);
		}

		///////////////////////////////////////////////////////////////////////

		uSize NextPowerOf2(uSize value)
		{
			uSize p = 1;
			if (value && !(value & (value - 1)))
				return value;

			while (p < value)
				p <<= 1;

			return p;
		}

		uSize NextGreaterPowerOf2(uSize value)
		{
			//unsigned long idx;
			//_BitScanReverse(&idx, (unsigned long)value);
			//return (uSize)(1U << (idx + 1));
			return NextPowerOf2(value + 1);
		}

		///////////////////////////////////////////////////////////////////////

	public:
		Table()
			: mTable(INITAL_SIZE, EntryType()), mSize(0), mCapacity(INITAL_SIZE),
			mThreshold(INITAL_SIZE* LOAD_FACTOR)
		{
			mTable[0].first = true;
			mTable[mCapacity - 1].last = true;
		}

		Table(HashType capacity)
			: mTable(capacity, EntryType()), mSize(0), mCapacity(capacity),
			mThreshold(capacity* LOAD_FACTOR)
		{
			mTable[0].first = true;
			mTable[mCapacity - 1].last = true;
		}

		Table(const Table& table)
			: mTable(table.mTable), mSize(table.mSize), mCapacity(table.mCapacity),
			mThreshold(table.mThreshold) { }

		Table(Table&& table) noexcept
			: Table()
		{
			Swap(*this, table);
		}

		template<typename RKeyValueType>
		KeyValueType& Insert(HashType hash, RKeyValueType&& keyValue)
		{
			if (mSize + 1 >= mThreshold)
			{
				ResizeRehash(NextGreaterPowerOf2(mCapacity));
			}

			return InsertImpl(hash, Forward<RKeyValueType>(keyValue)).keyValue;
		}

		void Remove(HashType hash, const KeyValueType& keyValue)
		{
			EntryType* pEntry = FindEntry(hash, keyValue);

			if (pEntry)
			{
				pEntry->used = false;
				mSize--;
			}
		}

		bool Reserve(HashType size)
		{
			if (size < mSize)
			{
				// Cannot resize
				return false;
			}

			ResizeRehash(NextPowerOf2(size));

			return true;
		}

		void Shrink()
		{
			HashType newSize = NextPowerOf2(mSize);

			if (newSize != mSize)
			{
				ResizeRehash(newSize);
			}
		}

		Iterator Find(HashType hash, const KeyValueType& keyValue)
		{
			EntryType* pEntry = FindEntry(hash, keyValue);
			return pEntry == nullptr ? End() : Iterator(pEntry);
		}

		ConstIterator Find(HashType hash, const KeyValueType& keyValue) const
		{
			EntryType* pEntry = FindEntry(hash, keyValue);
			return pEntry == nullptr ? End() : Iterator(pEntry);
		}

		template<typename RKeyValueType>
		KeyValueType& FindInsert(HashType hash, RKeyValueType&& keyValue)
		{
			EntryType& entry = FindInsertEntry(hash, Forward<RKeyValueType>(keyValue));
			return entry.keyValue;
		}

		bool Contains(HashType hash, const KeyValueType& keyValue)
		{
			return Find(hash, keyValue) != End();
		}

		Iterator Begin()
		{
			if (mTable.IsEmpty())
			{
				return nullptr;
			}

			Iterator it(&mTable[0]);

			if (mTable[0].IsEmpty())
			{
				return ++it;
			}

			return it;
		}

		ConstIterator Begin() const
		{
			if (mTable.IsEmpty())
			{
				return nullptr;
			}

			ConstIterator it(&mTable[0]);

			if (mTable[0].IsEmpty())
			{
				return ++it;
			}

			return it;
		}

		Iterator End()
		{
			if (mTable.IsEmpty())
			{
				return nullptr;
			}

			return Iterator(mTable.Data() + mCapacity);
		}

		ConstIterator End() const
		{
			if (mTable.IsEmpty())
			{
				return nullptr;
			}

			return ConstIterator(mTable.Data() + mCapacity);
		}

		void Clear()
		{
			mTable.Clear();
			mTable.Resize(mCapacity, EntryType());
			mTable[mCapacity - 1].isLast = true;
			mSize = 0;
		}

		EntryType* Data()
		{
			return mTable.Data();
		}

		HashType Size() const
		{
			return mSize;
		}

		HashType Capacity() const
		{
			return mCapacity;
		}

		HashType Threshold() const
		{
			return mThreshold;
		}

		bool IsEmpty() const
		{
			return mSize == 0;
		}

		Table& operator=(Table table)
		{
			Swap(*this, table);
			return *this;
		}

		// for-each functions:

		Iterator begin()
		{
			return Begin();
		}

		const ConstIterator begin() const
		{
			return Begin();
		}

		Iterator end()
		{
			return End();
		}
		
		const ConstIterator end() const
		{
			return End();
		}

		// Iterator overloads:

		friend Iterator& operator++(Iterator& itr)
		{
			do
			{
				if (itr.pItr->IsLast())
				{
					++itr.pItr;
					return itr;
				}

				++itr.pItr;
			} while (itr.pItr->IsEmpty());

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