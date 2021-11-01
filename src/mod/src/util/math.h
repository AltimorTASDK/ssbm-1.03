#pragma once

#include <algorithm>
#include <cmath>

namespace math {
constexpr auto pi = 3.14159265f;
constexpr auto tau = pi * 2;
}

constexpr auto align_up(auto value, auto alignment)
{
	const auto biased = value + alignment - 1;
	return biased - ((biased % alignment) + alignment) % alignment;
}

constexpr auto is_pow2(auto value)
{
	return value != 0 && (value & (value - 1)) == 0;
}

constexpr auto bit_swap(auto value, auto i, auto j, int count = 1)
{
	const auto x = ((value >> i) ^ (value >> j)) & ((1 << count) - 1);
	return value ^ ((x << i) | (x << j));
}

template<typename T>
constexpr T clamp(T value, T min, T max)
{
	return std::max(min, std::min(value, max));
}

template<typename T>
constexpr T mod(T value, T modulus)
{
	if (value >= 0)
		return value % modulus;

	// True modulo for negatives
	return ((value % modulus) + modulus) % modulus;
}

template<typename T>
constexpr T copysign_int(T value, T sign)
{
	const auto mask = sign >> (sizeof(T) * CHAR_BIT - 1);
	return (value + mask) ^ mask;
}

constexpr auto deg_to_rad(auto x)
{
	return x * math::pi / 180.f;
}

constexpr auto rad_to_deg(auto x)
{
	return x * 180.f / math::pi;
}

constexpr auto angle_difference(auto a, auto b)
{
	return std::fmod(deg_to_rad(a) - deg_to_rad(b) + math::pi, math::tau) - math::pi;
}