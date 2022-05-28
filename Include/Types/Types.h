#pragma once

namespace Quartz
{
	/*====================================================
	|	               QUARTZLIB TYPES                   |
	=====================================================*/

	typedef __int64				 int64;
	typedef __int32				 int32;
	typedef __int16				 int16;
	typedef __int8				 int8;

	typedef unsigned __int64	 uInt64;
	typedef unsigned __int32	 uInt32;
	typedef unsigned __int16	 uInt16;
	typedef unsigned __int8		 uInt8;

#ifdef QUARTZ_64

	typedef __int64				sSize;
	typedef unsigned __int64	uSize;

#else

	typedef __int32				sSize;
	typedef unsigned __int32	uSize;

#endif // QUARTZ_64

	typedef __wchar_t			wChar;

	typedef uInt8				flags8;
	typedef uInt16				flags16;
	typedef uInt32				flags32;
	typedef uInt64				flags64;

	typedef uInt8				handle8;
	typedef uInt16				handle16;
	typedef uInt32				handle32;
	typedef uInt64				handle64;
}