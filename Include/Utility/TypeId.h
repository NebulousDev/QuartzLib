#pragma once

#include "Types/Types.h"

namespace Quartz
{
    /*====================================================
    |                  QUARTZLIB TYPEID                  |
    =====================================================*/

    namespace Detail
    {
        // https://stackoverflow.com/questions/40993441/constexpr-tricks

	    template <typename T> 
        class Flag
        {
            struct Dummy
            {
                constexpr Dummy() { }
                friend constexpr void adl(Dummy);
            };

            template <bool>
            struct Writer
            {
                friend constexpr void adl(Dummy) {}
            };

            template <class Dummy, int = (adl(Dummy{}), 0) >
            static constexpr bool Check(int)
            {
                return true;
            }

            template <class Dummy>
            static constexpr bool Check(short)
            {
                return false;
            }

        public:

            template <class Dummy = Dummy, bool Value = Check<Dummy>(0)>
            static constexpr bool ReadSet()
            {
                Writer<Value && 0> tmp{};
                (void)tmp;
                return Value;
            }

            template <class Dummy = Dummy, bool Value = Check<Dummy>(0)>
            static constexpr int Read()
            {
                return Value;
            }
        };

        template <typename T, int I>
        struct Tag
        {

            constexpr int Value() const noexcept
            {
                return I;
            }
        };

        template<typename T, int N, bool B>
        struct Checker
        {
            static constexpr int CurrentVal() noexcept
            {
                return N;
            }
        };

        template<typename T, int N>
        struct CheckerWrapper
        {
            template<bool B = Flag<Tag<T, N>>{}.Read(), int M = Checker<T, N, B>{}.CurrentVal() >
            static constexpr int CurrentVal()
            {
                return M;
            }
        };

        template<typename T, int N>
        struct Checker<T, N, true>
        {
            template<int M = CheckerWrapper<T, N + 1>{}.CurrentVal() >
            static constexpr int CurrentVal() noexcept
            {
                return M;
            }
        };

        template<typename T, int N, bool B = Flag<Tag<T, N>>{}.ReadSet() >
        struct Next
        {
            static constexpr int Value() noexcept
            {
                return N;
            }
        };
    }

	template<typename Type>
	class TypeId
	{
	private:
		constexpr static uInt64 Hash(const char* signature)
		{
			const char* val = signature;
			uInt64 value = 14695981039346656037ull;

			while (*val != 0)
			{
				value = (value ^ static_cast<uInt64>(*(val++))) * 1099511628211ull;
			}

			return value;
		}

		template<uInt64 strSize>
		constexpr static uInt64 TypeHash(const char (&signature)[strSize])
		{
			return Hash(signature);
		}

	public:
		constexpr static uInt64 Value()
		{
			constexpr uInt64 value = static_cast<uInt64>(TypeHash(__FUNCSIG__));
			return value;
		}
	};

    template <class CounterTag = void>
    class TypeCounter
    {
    public:
        template <int N = Detail::CheckerWrapper<CounterTag, 0>{}.CurrentVal() >
        static constexpr int Next()
        {
            return Detail::Next<CounterTag, N>{}.Value();
        }
    };

	template<typename CategoryType, typename Type>
	class TypeIndex
	{
    private:
        constexpr static uInt64 mIndex = TypeCounter<CategoryType>::Next();

    public:
		constexpr static uInt64 Value()
		{
			return mIndex;
		}
	};
}