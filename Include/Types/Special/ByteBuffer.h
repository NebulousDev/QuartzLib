#pragma once

#include "Types/Array.h"
#include "Memory/Memory.h"

namespace Quartz
{
	/*====================================================
	|               QUARTZLIB BYTE BUFFER                |
	=====================================================*/

	template<uSize SMALL_SIZE = 0>
	class ByteBufferBase : protected Array<uInt8, SMALL_SIZE>
	{
	public:
		ByteBufferBase(uSize sizeBytes) : Array()
		{
			Reserve(sizeBytes);
		};

		template<typename ValueType>
		bool Write(const ValueType& value)
		{
			if (mSize + sizeof(ValueType) > mCapacity)
			{
				return false;
			}

			MemCopy((void*)&mpData[mSize], (void*)&value, sizeof(ValueType));
			mSize += sizeof(ValueType);

			return true;
		}

		using Array::Clear;

		using Array::Size;
		using Array::Capacity;

		using Array::Data;
	};

	using ByteBuffer = ByteBufferBase<0>;
}