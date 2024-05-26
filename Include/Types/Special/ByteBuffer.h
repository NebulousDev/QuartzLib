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

		bool WriteData(const void* pData, uSize sizeBytes)
		{
			if (mSize + sizeBytes > mCapacity)
			{
				return false;
			}

			MemCopy((void*)&mpData[mSize], pData, sizeBytes);
			mSize += sizeBytes;

			return true;
		}

		template<typename ValueType, uSize SMALL_SIZE>
		bool WriteArray(const Array<ValueType, SMALL_SIZE>& array)
		{
			WriteData((void*)array.Data(), array.Size() * sizeof(ValueType));
		}

		bool Allocate(uSize sizeBytes)
		{
			if (sizeBytes < mSize || sizeBytes > mCapacity)
			{
				return false;
			}

			mSize = sizeBytes;

			return true;
		}

		using Array::Clear;

		using Array::Size;
		using Array::Capacity;

		using Array::Data;
	};

	using ByteBuffer = ByteBufferBase<0>;
}