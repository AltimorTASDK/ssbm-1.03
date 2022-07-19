#pragma once

#include <algorithm>
#include <bit>
#include <climits>
#include <cmath>
#include <type_traits>

namespace math {

constexpr auto pi = 3.14159265f;
constexpr auto tau = pi * 2;

} // namespace math

constexpr auto align_down(auto value, auto alignment)
{
	return value - (value % alignment + alignment) % alignment;
}

constexpr auto align_up(auto value, auto alignment)
{
	return align_down(value + alignment - 1, alignment);
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
constexpr T lerp(T a, T b, auto c)
{
	return a + (b - a) * c;
}

template<typename T>
constexpr T inv_lerp(T value, T a, T b)
{
	if (value <= a)
		return (T) { 0 };
	else if (value >= b)
		return (T) { 1 };
	else
		return (value - a) / (b - a);
}

template<typename T>
constexpr T mod(T value, T modulus)
{
	if (value >= 0)
		return value % modulus;

	// True modulo for negatives
	return ((value % modulus) + modulus) % modulus;
}

// Returns mod(value - 1, modulus) given value is in range [0, modulus)
template<typename T>
constexpr T decrement_mod(T value, T modulus)
{
	constexpr auto shift = sizeof(T) * CHAR_BIT - 1;
	const auto dec = value - 1;
	const auto mask = std::bit_cast<std::make_signed_t<T>>(dec) >> shift;
	return dec ^ (-modulus & mask);
}

// Returns mod(value + 1, modulus) given value is in range [0, modulus)
template<typename T>
constexpr T increment_mod(T value, T modulus)
{
	constexpr auto shift = sizeof(T) * CHAR_BIT - 1;
	const auto inc = value - modulus + 1;
	const auto mask = std::bit_cast<std::make_signed_t<T>>(inc) >> shift;
	return modulus + (inc ^ (-modulus & ~mask));
}

template<typename T>
constexpr T copysign_int(T value, T sign)
{
	const auto mask = sign >> (sizeof(T) * CHAR_BIT - 1);
	return (value + mask) ^ mask;
}

constexpr float deg_to_rad(float x)
{
	return x * math::pi / 180.f;
}

constexpr float rad_to_deg(float x)
{
	return x * 180.f / math::pi;
}

constexpr auto angle_difference(auto a, auto b)
{
	return std::fmod(deg_to_rad(a) - deg_to_rad(b) + math::pi, math::tau) - math::pi;
}