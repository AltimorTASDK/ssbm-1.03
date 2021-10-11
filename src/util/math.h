#pragma once

namespace math {
constexpr auto pi = 3.14159265f;
constexpr auto tau = pi * 2;
}

constexpr auto align_up(auto value, auto alignment)
{
	// Powers of 2 only
	return (value + alignment - 1) & ~(alignment - 1);
}

constexpr auto is_pow2(auto value)
{
	return value != 0 && (value & (value - 1)) == 0;
}

template<typename T>
constexpr T copysign_int(T value, T sign)
{
	const auto mask = sign >> (sizeof(T) * CHAR_BIT - 1);
	return (value + mask) ^ mask;
}