#pragma once

#include "Types.h"
#include "Utility/Hash.h"
#include "Memory/Memory.h"
#include "Utility/Swap.h"
#include "Utility/Whitespace.h"

#include <cstring>

namespace Quartz
{
	/*====================================================
	|                  QUARTZLIB STRING                  |
	=====================================================*/

	template<typename CharType>
	uSize StrLen(const CharType* pStr);

	template<typename CharType>
	sSize StrCmp(const CharType* pStr1, const CharType* pStr2);

	template<typename CharType>
	sSize StrCmpCount(const CharType* pStr1, const CharType* pStr2, uSize count);

	template<typename CharType>
	const CharType* StrStr(const CharType* pStr, const CharType* pDelim);

	template<typename CharType>
	class WrapperStringBase;

	template<typename CharType>
	class SubstringBase;

	template<typename _CharType>
	class StringBase
	{
	public:
		using CharType			= _CharType;
		using SubstringType		= SubstringBase<CharType>;
		using WrapperStringType = WrapperStringBase<CharType>;

		using SubstringBase		= SubstringBase<CharType>;
		using WrapperStringBase = WrapperStringBase<CharType>;

	protected:

		struct StringMeta
		{
			uSize refCount;
			uSize length;

			StringMeta() :
				refCount(1), length(0) { }

			StringMeta(uSize refCount, uSize length) :
				refCount(refCount), length(length) { }
		};

		union
		{
			uInt8*		mpData;
			StringMeta* mpMeta;
		};

		static constexpr uSize metaSize = sizeof(StringMeta);

	private:
		friend void Swap(StringBase& str1, StringBase& str2)
		{
			using Quartz::Swap;
			Swap(str1.mpData, str2.mpData);
		}

	public:
		StringBase() : 
			mpData(new uInt8[metaSize + sizeof(CharType)])
		{
			*mpMeta = StringMeta(1, 0);
			*reinterpret_cast<CharType*>(mpData + metaSize) = '\0';
		}

		StringBase(const StringBase& str)
			: mpData(str.mpData)
		{
			++mpMeta->refCount;
		}

		StringBase(StringBase&& str) noexcept
			: StringBase()
		{
			Swap(*this, str);
		}

		StringBase(const CharType* pStr)
			: StringBase(pStr, StrLen(pStr)) { }

		StringBase(const CharType* pStr, uSize length)
		{
			const uSize strBufferSize	= (length + 1) * sizeof(CharType);
			const uSize fullBufferSize	= metaSize + strBufferSize;

			mpData = new uInt8[fullBufferSize]{};

			*mpMeta = StringMeta(1, length);
			MemCopy((void*)(mpData + metaSize), (void*)pStr, strBufferSize);

			// Set last value to zero (null-termination)
			reinterpret_cast<CharType*>(mpData + metaSize)[length] = '\0';
		}

		StringBase(const WrapperStringBase& str)
			: StringBase(str.Str(), str.Length()) { }

		StringBase(const SubstringBase& substr)
			: StringBase(substr.Str(), substr.Length()) { }

		~StringBase()
		{
			if (--mpMeta->refCount == 0)
			{
				delete[] mpData;
			}
		}

		StringBase Append(const WrapperStringBase& str) const
		{
			StringBase result;
			result.Resize(mpMeta->length + str.Length());
			MemCopy((void*)(result.mpData + metaSize), (void*)(mpData + metaSize), mpMeta->length);
			MemCopy((void*)(result.mpData + metaSize + mpMeta->length), (void*)str.Str(), str.Length() * sizeof(CharType));
			return result;
		}

		StringBase Append(const StringBase& str) const
		{
			StringBase result;
			result.Resize(mpMeta->length + str.mpMeta->length);
			MemCopy((void*)(result.mpData + metaSize), (void*)(mpData + metaSize), mpMeta->length);
			MemCopy((void*)(result.mpData + metaSize + mpMeta->length), (void*)str.Str(), str.mpMeta->length * sizeof(CharType));
			return result;
		}

		SubstringBase TrimWhitespace() const
		{
			return TrimWhitespaceForward().TrimWhitespaceReverse();
		}

		SubstringBase TrimWhitespaceForward() const
		{
			if (IsEmpty())
			{
				return *this;
			}

			uSize idx = 0;
			while (idx < mLength && IsWhitespace(Str()[idx]))
			{
				idx++;
			}

			return Substring(idx);
		}

		SubstringBase TrimWhitespaceReverse() const
		{
			if (IsEmpty())
			{
				return *this;
			}

			uSize idx = mLength - 1;
			while (idx > 0 && IsWhitespace(Str()[idx]))
			{
				idx--;
			}

			return Substring(0, idx + 1);
		}

		uSize Find(const WrapperStringBase& str) const
		{
			const CharType* foundStr = StrStr(Str(), str.Str());

			if (!foundStr)
			{
				return Length();
			}
			else
			{
				return (uSize)(foundStr - Str()) / sizeof(CharType);
			}
		}

		uSize FindReverse(const WrapperStringBase& str) const
		{
			const CharType* pStart = Str();
			const CharType* pSubStr = pStart + (Length() - 1);
			uSize revLength = 1;

			while (pSubStr != pStart)
			{
				WrapperStringBase testStr(pSubStr, revLength);

				if (testStr.StartsWith(str))
				{
					return Length() - revLength;
				}

				pSubStr--;
				revLength++;
			}

			return 0;
		}

		bool StartsWith(const WrapperStringBase& str) const
		{
			if (str.Length() > Length())
			{
				return false;
			}

			const CharType* pStr1 = Str();
			const CharType* pStr2 = str.Str();

			for (uSize i = 0; i < str.Length(); i++)
			{
				if (pStr1[i] != pStr2[i])
				{
					return false;
				}
			}

			return true;
		}

		SubstringBase Substring(uSize start, uSize end) const
		{
			return SubstringBase(*this, start, end);
		}

		SubstringBase Substring(uSize start) const
		{
			return SubstringBase(*this, start, Length());
		}

		bool operator==(const StringBase& str) const
		{
			return (Length() == str.Length()) &&
				(StrCmp(Str(), str.Str()) == 0);
		}

		bool operator==(const WrapperStringBase& str) const
		{
			return (Length() == str.Length()) &&
				(StrCmp(Str(), str.Str()) == 0);
		}

		bool operator==(const SubstringBase& substr) const
		{
			return (Length() == substr.Length()) &&
				(StrCmp(Str(), substr.Str()) == 0);
		}

		bool operator==(const CharType* pStr) const
		{
			return StrCmp(Str(), pStr) == 0;
		}

		bool operator!=(const StringBase& str) const
		{
			return !operator==(str);
		}

		bool operator!=(const WrapperStringBase& str) const
		{
			return !operator==(str);
		}

		bool operator!=(const SubstringBase& substr) const
		{
			return !operator==(substr);
		}

		bool operator!=(const CharType* pStr) const
		{
			return !operator==(pStr);
		}

		StringBase& operator=(StringBase str)
		{
			Swap(*this, str);
			return *this;
		}

		StringBase operator+(const CharType* pStr) const
		{
			return this->Append(WrapperStringBase(pStr));
		}

		StringBase operator+(const StringBase& str) const
		{
			return this->Append(str);
		}

		friend StringBase operator+(const CharType* pStr1, const StringBase& str2)
		{
			return String(pStr1).Append(str2);
		}

		StringBase& operator+=(const CharType* pStr)
		{
			*this = this->Append(pStr);
			return *this;
		}

		StringBase& operator+=(const StringBase& str)
		{
			*this = this->Append(str);
			return *this;
		}

		StringBase& Resize(uSize length)
		{
			const uSize strBufferSize = (length + 1) * sizeof(CharType);
			const uSize fullBufferSize = metaSize + strBufferSize;

			const uInt8* mpPrev = mpData;

			mpData = new uInt8[fullBufferSize]{};
			mpMeta->refCount = ((StringMeta*)mpPrev)->refCount;
			mpMeta->length = length;

			reinterpret_cast<CharType*>(mpData + metaSize)[length] = '\0';

			delete mpPrev;

			return *this;
		}

		const CharType* Str() const
		{
			return reinterpret_cast<const CharType*>(mpData + metaSize);
		}

		CharType* Data()
		{
			return reinterpret_cast<CharType*>(mpData + metaSize);
		}

		uSize Length() const
		{
			return mpMeta->length;
		}

		uSize RefCount() const
		{
			return mpMeta->refCount;
		}

		bool IsUnique() const
		{
			return mpMeta->refCount == 1;
		}

		bool IsEmpty() const
		{
			return mpMeta->length == 0;
		}
	};

	template<typename CharType>
	class WrapperStringBase
	{
	public:
		friend class StringBase<CharType>;

	public:
		using StringBase = Quartz::StringBase<CharType>;

	protected:
		uSize			mLength;
		const CharType*	mpStr;

	protected:
		friend void Swap(WrapperStringBase& wrapStr1, WrapperStringBase& wrapStr2)
		{
			using Quartz::Swap;
			Swap(wrapStr1.mLength, wrapStr2.mLength);

			// @TODO: figure out why a simple swap doesn't work on const char*s
			const CharType* temp = wrapStr1.mpStr;
			wrapStr1.mpStr = wrapStr2.mpStr;
			wrapStr2.mpStr = temp;
		}

	public:
		WrapperStringBase()
			: mLength(0), mpStr(nullptr) {}

		WrapperStringBase(const CharType* pStr)
			: mLength(StrLen<CharType>(pStr)), mpStr(pStr) {}

		WrapperStringBase(const CharType* pStr, uSize length)
			: mLength(length), mpStr(pStr) {}

		bool StartsWith(const StringBase& str) const
		{
			return StartsWith(WrapperStringBase(str.Str(), str.Length()))
		}

		bool StartsWith(const WrapperStringBase& str) const
		{
			if (str.Length() > Length())
			{
				return false;
			}

			const CharType* pStr1 = Str();
			const CharType* pStr2 = str.Str();

			for (uSize i = 0; i < str.Length(); i++)
			{
				if (pStr1[i] != pStr2[i])
				{
					return false;
				}
			}

			return true;
		}

		uSize Find(const WrapperStringBase& str) const
		{
			const CharType* foundStr = StrStr(Str(), str.Str());

			if (!foundStr)
			{
				return Length();
			}
			else
			{
				return (uSize)(foundStr - Str()) / sizeof(CharType);
			}
		}

		uSize FindReverse(const WrapperStringBase& str) const
		{
			const CharType* pStart = Str();
			const CharType* pSubStr = pStart + (Length() - 1);
			uSize revLength = 1;

			while (pSubStr != pStart)
			{
				WrapperStringBase testStr(pSubStr, revLength);

				if (testStr.StartsWith(str))
				{
					return Length() - revLength;
				}

				pSubStr--;
				revLength++;
			}

			return 0;
		}

		bool operator==(const StringBase& str) const
		{
			return (mLength == str.Length()) &&
				(StrCmpCount(Str(), str.Str(), mLength) == 0);
		}

		bool operator==(const WrapperStringBase& wrapStr) const
		{
			return (mLength == wrapStr.mLength) &&
				(StrCmpCount(Str(), wrapStr.Str(), mLength) == 0);
		}

		bool operator==(const CharType* pStr) const
		{
			return StrCmpCount(Str(), pStr, mLength) == 0;
		}

		bool operator!=(const StringBase& str) const
		{
			return !operator==(str);
		}

		bool operator!=(const WrapperStringBase& wrapStr) const
		{
			return !operator==(wrapStr);
		}

		bool operator!=(const CharType* pStr) const
		{
			return !operator==(pStr);
		}

		WrapperStringBase& operator=(WrapperStringBase wrapStr)
		{
			Swap(*this, wrapStr);
			return *this;
		}

		operator StringBase()
		{
			return String(Str(), mLength);
		}

		const CharType* Str() const
		{
			return reinterpret_cast<const CharType*>(mpStr);
		}

		CharType* Data()
		{
			return reinterpret_cast<CharType*>(mpStr);
		}

		operator const CharType* () { return mData; }
		operator CharType* () { return mData; }

		uSize Length() const
		{
			return mLength;
		}

		bool IsEmpty() const
		{
			return mLength == 0;
		}
	};

	template<typename CharType>
	class SubstringBase : public WrapperStringBase<CharType>
	{
	public:
		using StringBase		= Quartz::StringBase<CharType>;
		using WrapperStringBase	= Quartz::WrapperStringBase<CharType>;

	protected:
		// By keeping a copy of the original,
		// we guarantee the data is not deleted until
		// all substrings are destroyed
		StringBase mSrcString;

	protected:
		friend void Swap(SubstringBase& substr1, SubstringBase& substr2)
		{
			using Quartz::Swap;
			Swap(substr1.mLength, substr2.mLength);
			Swap(substr1.mSrcString, substr2.mSrcString);
			
			// @TODO: figure out why a simple swap doesn't work on const char*s
			const CharType* temp = substr1.mpStr;
			substr1.mpStr = substr2.mpStr;
			substr2.mpStr = temp;
		}

	public:
		SubstringBase()
			: WrapperStringBase(nullptr, 0), mSrcString() {}

		SubstringBase(const SubstringBase& substr)
			: WrapperStringBase(substr.mpStr, substr.mLength),
			mSrcString(substr.mSrcString) {}

		SubstringBase(SubstringBase&& rSubstr) noexcept
		{
			Swap(*this, rSubstr);
		}

		SubstringBase(const StringBase& str, uSize start, uSize end)
			: WrapperStringBase(str.Str() + start, end - start), 
			mSrcString(str) { }

		SubstringBase(const SubstringBase& substr, uSize start, uSize end)
			: WrapperStringBase(substr.Str() + start, end - start),
			mSrcString(substr.mSrcString) { }

		SubstringBase Substring(uSize start, uSize end) const
		{
			// @TODO assert? within bounds
			return SubstringBase(*this, start, end);
		}

		SubstringBase Substring(uSize start) const
		{
			return SubstringBase(*this, start, Length());
		}

		SubstringBase TrimWhitespace() const
		{ 
			return TrimWhitespaceForward().TrimWhitespaceReverse();
		}

		SubstringBase TrimWhitespaceForward() const
		{
			if (IsEmpty())
			{
				return *this;
			}

			uSize idx = 0;
			while (idx < mLength && IsWhitespace(mpStr[idx]))
			{
				idx++;
			}

			return Substring(idx);
		}

		SubstringBase TrimWhitespaceReverse() const
		{
			if (IsEmpty())
			{
				return *this;
			}

			uSize idx = mLength - 1;
			while (idx > 0 && IsWhitespace(mpStr[idx]))
			{
				idx--;
			}

			return Substring(0, idx + 1);
		}

		WrapperStringBase& operator=(WrapperStringBase wrapStr) = delete;

		SubstringBase& operator=(SubstringBase substr)
		{
			Swap(*this, substr);
			return *this;
		}
	};

	using StringA			= StringBase<char>;
	using StringW			= StringBase<wchar_t>;
	using WrapperStringA	= WrapperStringBase<char>;
	using WrapperStringW	= WrapperStringBase<wchar_t>;
	using SubstringA		= SubstringBase<char>;
	using SubstringW		= SubstringBase<wchar_t>;

	using String			= StringA;
	using WrapperString		= WrapperStringA;
	using Substring			= SubstringA;

	template<>
	inline uSize StrLen<char>(const char* pStr)
	{
		return static_cast<uSize>(strlen(pStr));
	}

	template<>
	inline uSize StrLen<wchar_t>(const wchar_t* pStr)
	{
		return static_cast<uSize>(wcslen(pStr));
	}

	template<>
	inline sSize StrCmp<char>(const char* pStr1, const char* pStr2)
	{
		return static_cast<sSize>(strcmp(pStr1, pStr2));
	}

	template<>
	inline sSize StrCmp<wchar_t>(const wchar_t* pStr1, const wchar_t* pStr2)
	{
		return static_cast<sSize>(wcscmp(pStr1, pStr2));
	}

	template<>
	inline sSize StrCmpCount<char>(const char* pStr1, const char* pStr2, uSize count)
	{
		return static_cast<sSize>(strncmp(pStr1, pStr2, count));
	}

	template<>
	inline sSize StrCmpCount<wchar_t>(const wchar_t* pStr1, const wchar_t* pStr2, uSize count)
	{
		return static_cast<sSize>(wcsncmp(pStr1, pStr2, count));
	}

	template<>
	inline const char* StrStr<char>(const char* pStr, const char* pDelim)
	{
		return strstr(pStr, pDelim);
	}

	template<>
	inline const wchar_t* StrStr<wchar_t>(const wchar_t* pStr, const wchar_t* pDelim)
	{
		return wcsstr(pStr, pDelim);
	}

	template<>
	inline uSize Hash<StringA>(const StringA& value)
	{
		// MurmurOAAT64 Hash

		const char* pStr = value.Str();

		uInt64 hash = 525201411107845655ull;

		while (*(pStr++))
		{
			hash ^= *pStr;
			hash *= 0x5bd1e9955bd1e995;
			hash ^= hash >> 47;
		}

		return static_cast<uSize>(hash);
	}

	template<>
	inline uSize Hash<StringW>(const StringW& value)
	{
		// MurmurOAAT64 Hash

		const wchar_t* pStr = value.Str();

		uInt64 hash = 525201411107845655ull;

		while (*(pStr++))
		{
			hash ^= *pStr;
			hash *= 0x5bd1e9955bd1e995;
			hash ^= hash >> 47;
		}

		return static_cast<uSize>(hash);
	}

	// @Todo: Speed up
	inline StringW StringAToStringW(const StringA& strA)
	{
		StringW wide;
		wide.Resize(strA.Length());

		char* pStr = const_cast<StringA&>(strA).Data();
		wchar_t* pWide = wide.Data();

		while (*pStr)
		{
			*pWide = (wchar_t)*pStr;
			++pWide;
			++pStr;
		}

		return wide;
	}

	// @Todo: Speed up
	inline StringA StringWToStringA(const StringW& strW)
	{
		StringA ascii;
		ascii.Resize(strW.Length());

		wchar_t* pStr = const_cast<StringW&>(strW).Data();
		char* pAscii = ascii.Data();

		while (*pStr)
		{
			*pAscii = *reinterpret_cast<char*>(pStr);
			++pAscii;
			++pStr;
		}

		return ascii;
	}

	inline String operator"" _STR(const char* str, size_t size)
	{
		return String(str, size);
	}

	inline StringW operator"" _WSTR(const wchar_t* str, size_t size)
	{
		return StringW(str, size);
	}
}