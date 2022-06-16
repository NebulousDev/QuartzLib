#pragma once

namespace Quartz
{
	/*====================================================
	|                QUARTZLIB ITERATOR                  |
	=====================================================*/

	template<typename IterType, typename ValueType>
	class Iterator
	{
	public:
		ValueType* pItr;

	public:
		Iterator()
			: pItr(nullptr) { }

		Iterator(ValueType* pItr)
			: pItr(pItr) { }

		Iterator(const Iterator& it)
			: pItr(it.pItr) { }

		ValueType& operator*()
		{
			return *pItr;
		}

		ValueType* operator->()
		{
			return pItr;
		}
	};

	template<typename IterType, typename ValueType>
	class ConstIterator : public Iterator<IterType, ValueType>
	{
	private:
		using Iterator<IterType, ValueType>::pItr;

	public:
		ConstIterator()
			: Iterator<IterType, ValueType>() { }

		ConstIterator(ValueType* pItr)
			: Iterator<IterType, ValueType>(pItr) { }

		ConstIterator(const ConstIterator& it)
			: Iterator<IterType, ValueType>(it) { }

		const ValueType& operator*() const
		{
			return *pItr;
		}

		const ValueType* operator->() const
		{
			return pItr;
		}
	};

	template<typename IterType, typename ValueType>
	Iterator<IterType, ValueType>& 
		operator++(Iterator<IterType, ValueType>& itr)
	{
		++itr.pItr;
		return itr;
	}

	template<typename IterType, typename ValueType>
	Iterator<IterType, ValueType> 
		operator++(Iterator<IterType, ValueType>& itr, int)
	{
		Iterator<IterType, ValueType> temp(itr);
		++itr.pItr;
		return temp;
	}

	template<typename IterType, typename ValueType>
	const Iterator<IterType, ValueType>& 
		operator++(const Iterator<IterType, ValueType>& itr)
	{
		++itr.pItr;
		return itr;
	}

	template<typename IterType, typename ValueType>
	const Iterator<IterType, ValueType> 
		operator++(const Iterator<IterType, ValueType>& itr, int)
	{
		Iterator<IterType, ValueType> temp(itr);
		++itr.pItr;
		return temp;
	}

	template<typename IterType, typename ValueType>
	bool operator==(const Iterator<IterType, ValueType>& itrLeft, 
		const Iterator<IterType, ValueType>& itrRight)
	{
		return itrLeft.pItr == itrRight.pItr;
	}

	template<typename IterType, typename ValueType>
	bool operator!=(const Iterator<IterType, ValueType>& itrLeft, 
		const Iterator<IterType, ValueType>& itrRight)
	{
		return itrLeft.pItr != itrRight.pItr;
	}

	template<typename IterType, typename ValueType>
	bool operator>=(const Iterator<IterType, ValueType>& itrLeft, 
		const Iterator<IterType, ValueType>& itrRight)
	{
		return itrLeft.pItr >= itrRight.pItr;
	}

	template<typename IterType, typename ValueType>
	bool operator<=(const Iterator<IterType, ValueType>& itrLeft, 
		const Iterator<IterType, ValueType>& itrRight)
	{
		return itrLeft.pItr <= itrRight.pItr;
	}

	template<typename IterType, typename ValueType>
	bool operator>(const Iterator<IterType, ValueType>& itrLeft, 
		const Iterator<IterType, ValueType>& itrRight)
	{
		return itrLeft.pItr > itrRight.pItr;
	}

	template<typename IterType, typename ValueType>
	bool operator<(const Iterator<IterType, ValueType>& itrLeft, 
		const Iterator<IterType, ValueType>& itrRight)
	{
		return itrLeft.pItr < itrRight.pItr;
	}
}