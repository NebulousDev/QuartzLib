#pragma once

#include "Types.h"
#include "Utility/Swap.h"
#include "Utility/Iterator.h"

namespace Quartz
{
	/*====================================================
	|	                QUARTZLIB LIST                   |
	=====================================================*/

	template<typename ValueType>
	class List
	{
	public:
		using Iterator		= Quartz::Iterator<List, ValueType>;
		using ConstIterator = Quartz::ConstIterator<List, ValueType>;

	private:
		struct ListNode
		{
			ValueType value;
			ListNode* pPrev;
			ListNode* pNext;

			ListNode(const ValueType& value, ListNode* pPrev, ListNode* pNext)
				: value(value), pPrev(pPrev), pNext(pNext) { }

			ListNode(ValueType&& value, ListNode* pPrev, ListNode* pNext)
				: value(Move(value)), pPrev(pPrev), pNext(pNext) { }
		};

		ListNode*	mpHead;
		ListNode*	mpTail;
		uSize		mSize;

	private:
		friend void Swap(List& list1, List& list2)
		{
			using Quartz::Swap;
			Swap(list1.mpHead, list2.mpHead);
			Swap(list1.mpTail, list2.mpTail);
			Swap(list1.mSize, list2.mSize);
		}

	public:
		List()
			: mpHead(nullptr), mpTail(nullptr), mSize(0) {}

		List(const List& list)
			: mpHead(list.mpHead), mpTail(nullptr), mSize(0) {}

		List(List&& list)
		{
			Swap(*this, list);
		}

		ValueType* Get(uSize index)
		{
			if (mSize < index)
			{
				return nullptr;
			}

			ListNode* pNode = mpHead;

			for (uSize i = 0; i < index; i++)
			{
				pNode = pNode->pNext;
			}

			return &pNode->value;
		}

		template<typename RValueType>
		ValueType& PushHead(RValueType&& value)
		{
			ListNode* pNewNode = new ListNode(Forward<RValueType>(value), nullptr, mpHead);

			if (mpHead)
			{
				mpHead->pPrev = pNewNode;
			}

			mpHead = pNewNode;

			++mSize;

			if (!mpTail)
			{
				mpTail = mpHead;
			}

			return pNewNode->value;
		}

		template<typename RValueType>
		ValueType& PushTail(RValueType&& value)
		{
			ListNode* pNewNode = new ListNode(Forward<RValueType>(value), mpTail, nullptr);

			if (mpTail)
			{
				mpTail->pNext = pNewNode;
			}

			mpTail = pNewNode;

			++mSize;

			if (!mpHead)
			{
				mpHead = mpTail;
			}

			return pNewNode->value;
		}

		template<typename RValueType>
		ValueType* Insert(uSize index, RValueType&& value)
		{
			if (mSize < index)
			{
				return nullptr;
			}

			ListNode* pNode = mpHead;

			for (uSize i = 0; i < index; i++)
			{
				pNode = pNode->pNext;
			}

			ListNode* pNewNode = new ListNode(Forward<RValueType>(value), pNode->pPrev, pNode);

			if (pNode->pPrev)
			{
				pNode->pPrev->pNext = pNewNode;
			}
			
			pNode->pPrev = pNewNode;

			return &pNode->value;
		}

		ValueType PopHead()
		{
			if (mpHead != nullptr)
			{
				ValueType value = Move(mpHead->value);

				ListNode* pOldHead = mpHead;
				mpHead = mpHead->pNext;

				if (mpHead != nullptr)
				{
					mpHead->pPrev = nullptr;
				}

				--mSize;

				delete pOldHead;

				return value;
			}

			// @Todo: throw error
			return ValueType();
		}

		ValueType PopTail()
		{
			if (mpTail != nullptr)
			{
				ValueType value = Move(mpHead->value);
				 
				ListNode* pOldTail = mpTail;
				mpTail = mpTail->pPrev;

				if (mpTail != nullptr)
				{
					mpTail->pNext = nullptr;
				}

				--mSize;

				delete pOldTail;

				return value;
			}

			// @Todo: throw error
			return ValueType();
		}

		ValueType PopIndex(uSize index)
		{
			if (mSize < index)
			{
				// @Todo: throw error
				return ValueType();
			}

			ListNode* pNode = mpHead;

			for (uSize i = 0; i < index; i++)
			{
				pNode = pNode->pNext;
			}

			ValueType value = Move(pNode->value);

			if (pNode->pNext)
			{
				pNode->pNext->pPrev = pNode->pPrev;
			}

			if (pNode->pPrev)
			{
				pNode->pPrev->pNext = pNode->pNext;
			}

			return value;
		}

		ValueType* GetHead()
		{
			return mpHead != nullptr ? &mpHead->value : nullptr;
		}

		ValueType* GetTail()
		{
			return mpTail != nullptr ? &mpTail->value : nullptr;
		}

		uSize Size()
		{
			return mSize;
		}
	};
}