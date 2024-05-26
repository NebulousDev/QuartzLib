#pragma once

#include "Types/Types.h"
#include "Utility/Move.h"
#include "Utility/Swap.h"

namespace Quartz
{
	/*====================================================
	|               QUARTZLIB REF COUNTER                |
	=====================================================*/

	template<typename ValueType>
	class RefCounted
	{
	public:
		struct RefValue
		{
			uSize		mRefCount;
			ValueType	mValue;

			//RefValue(const RefValue& refValue) :
			//	mRefCount(refValue.mRefCount), mValue(refValue.mValue) {}

			RefValue(const ValueType& rvalue) :
				mRefCount(1), mValue(value) {}

			RefValue(ValueType&& rvalue) :
				mRefCount(1), mValue(Move(rvalue)) {}

			bool IsUnique() const { return mRefCount == 1; }
		};

	private:
		RefValue* pRefValue;

	public:
		RefCounted(const ValueType& value) :
			pRefValue(new RefValue(value)) {}

		RefCounted(ValueType&& rvalue) :
			pRefValue(new RefValue(Move(rvalue))) {}

		RefCounted(const RefCounted& refCounted) :
			pRefValue(refCounted.pRefValue)
		{
			pRefValue->mRefCount++;
		}

		RefCounted(RefCounted&& rrefCounted) :
			pRefValue(refCounted.pRefValue)
		{
			pRefValue->mRefCount++;
		}

		~RefCounted()
		{
			if (pRefValue->IsUnique())
			{
				delete pRefValue;
			}
			else
			{
				pRefValue->mRefCount--;
			}
		}

		ValueType& operator->()
		{
			return pRefValue->mValue;
		}

		RefCounted& operator=(const ValueType& value)
		{
			pRefValue->mValue = value;
			return *this;
		}

		RefCounted& operator=(ValueType&& rvalue)
		{
			pRefValue->mValue = Move(rvalue);
			return *this;
		}

		RefCounted& operator=(RefCounted refCounted)
		{
			Swap(pRefValue, rrefCounted.pRefValue);
			return *this;
		}
	};
}