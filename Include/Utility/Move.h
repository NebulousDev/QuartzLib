#pragma once

namespace Quartz
{
	/*====================================================
	|	                QUARTZLIB MOVE                   |
	=====================================================*/

	template<typename _Type>
	struct RValue
	{
		using Type = _Type;
	};

	template<typename _Type>
	struct RValue<_Type&>
	{
		using Type = _Type;
	};

	template<typename _Type>
	struct RValue<_Type&&>
	{
		using Type = _Type;
	};

	template<typename _Type>
	using StripReference = typename RValue<_Type>::Type;

	template<typename Type>
	constexpr Type&& Forward(StripReference<Type>& value) noexcept
	{
		return static_cast<Type&&>(value);
	}

	template<typename Type>
	constexpr Type&& Forward(StripReference<Type>&& value) noexcept
	{
		return static_cast<Type&&>(value);
	}

	template<typename Type>
	constexpr StripReference<Type>&& Move(Type&& value) noexcept
	{
		return static_cast<StripReference<Type>&&>(value);
	}
}