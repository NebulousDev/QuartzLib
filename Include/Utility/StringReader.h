#pragma once

#include "Types/String.h"

namespace Quartz
{
	/*====================================================
	|                   STRING READER                    |
	=====================================================*/

	template<typename StringType>
	class StringReader
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
		StringReader(const StringType& string) :
			mString(string), mCursor(0), mRemaining(string.Substring(0)) { }

		const CharType& Read()
		{
			const CharType& readChar = mString.Str()[mCursor++];
			mRemaining = mString.Substring(mCursor);
			return readChar;
		}

		const SubstringType& ReadTo(const WrapperType& delim)
		{
			uSize nextIdx = mCursor + mRemaining.Find(delim);
			Substring line = mString.Substring(mCursor, nextIdx);
			mCursor = nextIdx + delim.Length();
			mRemaining = mString.Substring(mCursor);

			return line;
		}

		const SubstringType& ReadLine()
		{
			SubstringType line = ReadTo("\n");

			if (line.Str()[line.Length() - 1] == '\r')
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

		void Reset()
		{
			mCursor = 0;
		}

		bool SetCursor(uSize index)
		{
			if (index < mString.Length())
			{
				return false;
			}

			mCursor = index;

			return true;
		}

		bool IsEmpty() const { return mCursor >= mString.Length(); }
		const SubstringType& GetRemaining() const { return mRemaining; }
		uSize GetCursor() const { return mCursor; }
	};
}