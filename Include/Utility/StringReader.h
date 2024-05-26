#pragma once

#include "Types/String.h"
#include <string.h>

namespace Quartz
{
	/*====================================================
	|                   STRING READER                    |
	=====================================================*/

	template<typename CharType>
	float ReadFloat(const CharType* pStr, CharType** ppEnd) = delete;

	template<>
	inline float ReadFloat<char>(const char* pStr, char** ppEnd)
	{
		return strtof(pStr, ppEnd);
	}

	template<>
	inline float ReadFloat<wchar_t>(const wchar_t* pStr, wchar_t** ppEnd)
	{
		return wcstof(pStr, ppEnd);
	}

	template<typename CharType>
	int64 ReadInt(const CharType* pStr, CharType** ppEnd) = delete;

	template<>
	inline int64 ReadInt<char>(const char* pStr, char** ppEnd)
	{
		return strtol(pStr, ppEnd, 0);
	}

	template<>
	inline int64 ReadInt<wchar_t>(const wchar_t* pStr, wchar_t** ppEnd)
	{
		return wcstol(pStr, ppEnd, 0);
	}

	template<typename StringType>
	class StringReaderBase
	{
	public:
		using CharType		= typename StringType::CharType;
		using SubstringType = typename StringType::SubstringType;
		using WrapperType	= typename StringType::WrapperStringType;

	private:
		StringType		mString;
		SubstringType	mRemaining;
		uSize			mCursor;

	public:
		StringReaderBase(const StringType& string) :
			mString(string), mCursor(0), mRemaining(string.Substring(0)) { }

		const CharType& Peek()
		{
			return mString.Str()[mCursor];
		}

		const CharType& Read()
		{
			if(mCursor + 1 > mString.Length())
			{
				return mString.Str()[mString.Length()];
			}

			const CharType& readChar = mString.Str()[mCursor++];
			mRemaining = mString.Substring(mCursor);
			return readChar;
		}

		SubstringType ReadThrough(const WrapperType& delim)
		{
			if (IsEmpty())
			{
				return Substring();
			}

			// @TODO: check this isn't a problem .Str()
			uSize nextIdx = mCursor + mRemaining.Find(delim.Str()); 
			Substring line = mString.Substring(mCursor, nextIdx);
			mCursor = nextIdx + delim.Length();

			if (mCursor > mString.Length())
			{
				mCursor = mString.Length();
			}

			mRemaining = mString.Substring(mCursor);

			return line;
		}

		SubstringType ReadTo(const WrapperType& delim)
		{
			if (IsEmpty())
			{
				return Substring();
			}

			// @TODO: check this isn't a problem .Str()
			uSize nextIdx = mCursor + mRemaining.Find(delim.Str());
			Substring line = mString.Substring(mCursor, nextIdx);
			mCursor = nextIdx;

			if (mCursor > mString.Length())
			{
				mCursor = mString.Length();
			}

			mRemaining = mString.Substring(mCursor);

			return line;
		}

		SubstringType ReadLine()
		{
			SubstringType line = ReadThrough("\n");

			if (!line.IsEmpty() && line.Str()[line.Length() - 1] == '\r')
			{
				return line.Substring(0, line.Length() - 1);
			}

			return line;
		}

		void SkipWhitespace()
		{
			const CharType* pStr = &(mString.Str()[mCursor]);

			while (IsWhitespace(*pStr))
			{
				pStr++;
				mCursor++;
			}

			mRemaining = mString.Substring(mCursor);
		}

		float ReadFloat()
		{
			CharType* pRead = nullptr;
			const CharType* pStr = &(mString.Str()[mCursor]);
			float value = Quartz::ReadFloat<CharType>(pStr, &pRead);
			SetCursor(pRead - mString.Str());

			return value;
		}

		int64 ReadInt()
		{
			CharType* pRead = nullptr;
			const CharType* pStr = &(mString.Str()[mCursor]);
			int64 value = Quartz::ReadInt<CharType>(pStr, &pRead);
			SetCursor(pRead - mString.Str());

			return value;
		}

		void Reset()
		{
			mCursor = 0;
		}

		bool SetCursor(uSize index)
		{
			if (index > mString.Length())
			{
				return false;
			}

			mCursor = index;
			mRemaining = mString.Substring(mCursor);

			return true;
		}

		bool IsEmpty() const { return mCursor >= mString.Length(); }
		bool IsEnd() const { return IsEmpty(); }
		const SubstringType& GetRemaining() const { return mRemaining; }
		uSize GetCursor() const { return mCursor; }
	};

	using StringReaderA = StringReaderBase<StringA>;
	using StringReaderW = StringReaderBase<StringW>;
	using StringReader = StringReaderA;
}