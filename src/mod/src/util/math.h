#pragma once

#include <algorithm>
#include <bit>
#include <climits>
#include <concepts>
#include <cmath>
#include <type_traits>

namespace math {

constexpr auto pi = 3.14159265f;
constexpr auto tau = pi * 2;

} // namespace math

template<std::signed_integral T>
constexpr T mod(T value, T modulus)
{
	// True modulo for negatives
	return ((value % modulus) + modulus) % modulus;
}

template<std::unsigned_integral T>
constexpr T mod(T value, T modulus)
{
	return value % modulus;
}

constexpr auto align_down(std::integral auto value, std::integral auto alignment)
{
	return value - mod(value, alignment);
}

constexpr auto align_up(std::integral auto value, std::integral auto alignment)
{
	return align_down(value + alignment - 1, alignment);
}

constexpr auto bit_swap(std::integral auto value, int i, int j, int count = 1)
{
	const auto x = ((value >> i) ^ (value >> j)) & ((1 << count) - 1);
	return (decltype(value))(value ^ ((x << i) | (x << j)));
}

template<typename T>
constexpr T clamp(T value, T min, T max)
{
	return std::max(min, std::min(value, max));
}

template<typename T> requires (!std::integral<T>)
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

// Returns mod(value - 1, modulus) given value is in range [0, modulus)
template<std::integral T>
constexpr T decrement_mod(T value, T modulus)
{
	constexpr auto shift = sizeof(T) * CHAR_BIT - 1;
	const auto dec = value - 1;
	const auto mask = std::bit_cast<std::make_signed_t<T>>(dec) >> shift;
	return dec ^ (-modulus & mask);
}

// Returns mod(value + 1, modulus) given value is in range [0, modulus)
template<std::integral T>
constexpr T increment_mod(T value, T modulus)
{
	constexpr auto shift = sizeof(T) * CHAR_BIT - 1;
	const auto inc = value - modulus + 1;
	const auto mask = std::bit_cast<std::make_signed_t<T>>(inc) >> shift;
	return modulus + (inc ^ (-modulus & ~mask));
}

template<std::integral T>
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

constexpr float angle_difference(float a, float b)
{
	return std::fmod(deg_to_rad(a) - deg_to_rad(b) + math::pi, math::tau) - math::pi;
}