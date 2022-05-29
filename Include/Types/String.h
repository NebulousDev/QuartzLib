#pragma once

#include "Types.h"
#include "Utility/Hash.h"
#include "Memory/Memory.h"

#include <cstring>

namespace Quartz
{
	/*====================================================
	|	               QUARTZLIB STRING                  |
	=====================================================*/

	template<typename CharType>
	uSize StrLen(const CharType* pStr);

	template<typename CharType>
	uSize StrCmp(const CharType* pStr1, const CharType* pStr2);

	template<typename CharType>
	uSize StrCmpCount(const CharType* pStr1, const CharType* pStr2, uSize count);

	template<typename CharType>
	class SubstringBase;

	template<typename CharType>
	class StringBase
	{
	public:
		using SubstringType = SubstringBase<CharType>;

		friend SubstringType;

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
			mpMeta(new StringMeta()) { }

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

			mpData = new uInt8[fullBufferSize];

			*mpMeta = StringMeta(1, length);
			MemCopy((void*)(mpData + metaSize), (void*)pStr, strBufferSize);

			// Set last value to zero (null-termination)
			reinterpret_cast<CharType*>(mpData + metaSize)[length] = '\0';
		}

		StringBase(const SubstringType& substr)
			: StringBase(substr.Str(), substr.Length()) { }

		~StringBase()
		{
			if (--mpMeta->refCount == 0)
			{
				delete mpData;
			}
		}

		StringBase Append(const CharType* pStr) const
		{
			StringBase result;
			uSize length = StrLen(pStr);
			result.Resize(mpMeta->length + length);
			MemCopy((void*)(result.mpData + metaSize), (void*)(mpData + metaSize), mpMeta->length);
			MemCopy((void*)(result.mpData + metaSize + mpMeta->length), (void*)pStr, length * sizeof(CharType));
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

		SubstringType Substring(uSize start, uSize end)
		{
			return SubstringType(*this, start, end);
		}

		bool operator==(const StringBase& str) const
		{
			return (Length() == str.Length()) &&
				(StrCmp(Str(), str.Str()) == 0);
		}

		bool operator==(const SubstringType& substr) const
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

		bool operator!=(const SubstringType& substr) const
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
			return this->Append(pStr);
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

			mpData = new uInt8[fullBufferSize];
			mpMeta->refCount = ((StringMeta*)mpPrev)->refCount;
			mpMeta->length = length;

			reinterpret_cast<CharType*>(mpData + metaSize)[length] = 0;

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

		uSize RefrefCount() const
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
	class SubstringBase
	{
	public:
		friend class StringBase<CharType>;

	public:
		using StringBase	= Quartz::StringBase<CharType>;
		using SubstringType = Quartz::SubstringBase<CharType>;

	protected:
		uSize	mLength;
		uInt8*	mpData;

		// By keeping a copy of the original,
		// we guarantee the data is not deleted until
		// all substrs are destroyed
		StringBase	mSrcString;

	protected:
		friend void Swap(SubstringType& substr1, SubstringType& substr2)
		{
			using Quartz::Swap;
			Swap(substr1.mLength, substr2.mLength);
			Swap(substr1.mpData, substr2.mpData);
			Swap(substr1.mSrcString, substr2.mSrcString);
		}

	public:
		SubstringBase()
			: mLength(0), mpData(nullptr), mSrcString() {}

		SubstringBase(const SubstringType& substr)
			: mLength(substr.mLength), mpData(substr.mpData), mSrcString(substr.mSrcString) {}

		SubstringBase(SubstringType&& rSubstr) noexcept
		{
			Swap(*this, rSubstr);
		}

		SubstringBase(StringBase& str, uSize start, uSize end)
			: mSrcString(str)
		{
			mLength = end - start;
			mpData = reinterpret_cast<uInt8*>(str.Data() + start);
		}

		SubstringBase(SubstringType& substr, uSize start, uSize end)
			: mSrcString(substr.mSrcString)
		{
			mLength = end - start;
			mpData = reinterpret_cast<uInt8*>(substr.Data() + start);
		}

		bool operator==(const StringBase& str) const
		{
			return (mLength == str.Length()) &&
				(StrCmpCount(Str(), str.Str(), mLength) == 0);
		}

		bool operator==(const SubstringType& substr) const
		{
			return (mLength == substr.mLength) &&
				(StrCmpCount(Str(), substr.Str(), mLength) == 0);
		}

		bool operator==(const CharType* pStr) const
		{
			return StrCmpCount(Str(), pStr, mLength) == 0;
		}

		bool operator!=(const StringBase& str) const
		{
			return !operator==(str);
		}

		bool operator!=(const SubstringType& substr) const
		{
			return !operator==(substr);
		}

		bool operator!=(const CharType* pStr) const
		{
			return !operator==(pStr);
		}

		SubstringType& operator=(SubstringType substr)
		{
			Swap(*this, substr);
			return *this;
		}

		operator StringBase()
		{
			return String(Str(), mLength);
		}

		uSize Hash() const
		{
			return StringHash(Str());
		}

		const CharType* Str() const
		{
			return reinterpret_cast<const CharType*>(mpData);
		}

		CharType* Data()
		{
			return reinterpret_cast<CharType*>(mpData);
		}

		uSize Length() const
		{
			return mLength;
		}
	};

	using StringA		= StringBase<char>;
	using StringW		= StringBase<wchar_t>;
	using SubStringA	= SubstringBase<char>;
	using SubStringW	= SubstringBase<wchar_t>;

	using String		= StringA;
	using SubString		= SubStringA;

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
	inline uSize StrCmp<char>(const char* pStr1, const char* pStr2)
	{
		return static_cast<uSize>(strcmp(pStr1, pStr2));
	}

	template<>
	inline uSize StrCmp<wchar_t>(const wchar_t* pStr1, const wchar_t* pStr2)
	{
		return static_cast<uSize>(wcscmp(pStr1, pStr2));
	}

	template<>
	inline uSize StrCmpCount<char>(const char* pStr1, const char* pStr2, uSize count)
	{
		return static_cast<uSize>(strncmp(pStr1, pStr2, count));
	}

	template<>
	inline uSize StrCmpCount<wchar_t>(const wchar_t* pStr1, const wchar_t* pStr2, uSize count)
	{
		return static_cast<uSize>(wcsncmp(pStr1, pStr2, count));
	}

	template<>
	inline uSize Hash<String>(const String& value)
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