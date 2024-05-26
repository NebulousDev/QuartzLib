#pragma once

#include "Types/Types.h"

#define QUARTZ_HASH_64
#define QUARTZ_HASH_INVALID 0

namespace Quartz
{
	/*====================================================
	|                   QUARTZLIB HASH                   |
	=====================================================*/

	using hash64 = uInt64;

	template<typename HashType>
	hash64 Hash(const HashType& value) = delete;

	template<>
	inline hash64 Hash<uInt32>(const uInt32& value)
	{
		// https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key

#ifdef QUARTZ_HASH_64

		uInt64 value64 = value;

		value64 = (value64 ^ (value64 >> 31) ^ (value64 >> 62)) * uInt64(0x319642b2d24d8ec3);
		value64 = (value64 ^ (value64 >> 27) ^ (value64 >> 54)) * uInt64(0x96de1b173f119089);
		value64 = value64 ^ (value64 >> 30) ^ (value64 >> 60);

		return value64;
#else
		uInt32 value32 = value;

		value32 = ((value32 >> 16) ^ value32) * 0x45d9f3b;
		value32 = ((value32 >> 16) ^ value32) * 0x45d9f3b;
		value32 = ( value32 >> 16) ^ value32;

		return value;

#endif // QUARTZ_64
	}

	template<>
	inline hash64 Hash<int32>(const int32& value)
	{
		return Hash<uInt32>((const uInt32&)value);
	}

	template<>
	inline hash64 Hash<uInt64>(const uInt64& value)
	{
		// https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key

		uInt64 value64 = value;

		value64 = (value64 ^ (value64 >> 31) ^ (value64 >> 62)) * uInt64(0x319642b2d24d8ec3);
		value64 = (value64 ^ (value64 >> 27) ^ (value64 >> 54)) * uInt64(0x96de1b173f119089);
		value64 =  value64 ^ (value64 >> 30) ^ (value64 >> 60);

		return value64;
	}
}