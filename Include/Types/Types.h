#pragma once

#ifndef QUARTZ_TYPES

#include <cinttypes>

namespace Quartz
{
	/*====================================================
	|                  QUARTZLIB TYPES                   |
	=====================================================*/

#if defined(_MSC_VER)

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

#else // GCC / OTHER

	typedef signed long long	int64;
	typedef signed int			int32;
	typedef signed short		int16;
	typedef signed char			int8;

	typedef unsigned long long	uInt64;
	typedef unsigned int		uInt32;
	typedef unsigned short		uInt16;
	typedef unsigned char		uInt8;

#ifdef QUARTZ_64

	typedef signed long long	sSize;
	typedef unsigned long long	uSize;

#else

	typedef signed int			sSize;
	typedef unsigned int		uSize;

#endif // QUARTZ_64

	typedef unsigned short		wChar;

#endif

	typedef uInt8				flags8;
	typedef uInt16				flags16;
	typedef uInt32				flags32;
	typedef uInt64				flags64;

	typedef uInt8				handle8;
	typedef uInt16				handle16;
	typedef uInt32				handle32;
	typedef uInt64				handle64;
}

#endif // !QUARTZ_TYPES
#define QUARTZ_TYPES