#pragma once

#include <functional>
#include <type_traits>

template<typename T>
concept ScopedEnum = std::is_scoped_enum_v<T>;

template<typename T, typename U>
constexpr auto enum_operator_impl(auto &&callable, T a, U b)
{
	using enum_t = std::conditional_t<ScopedEnum<T>, T, U>;
	return enum_t(callable(std::to_underlying(enum_t{a}), std::to_underlying(enum_t{b})));
}

constexpr auto operator+(auto a, auto b)
	requires (ScopedEnum<decltype(a)> || ScopedEnum<decltype(b)>)
	{ return enum_operator_impl(std::plus(), a, b); }
constexpr auto operator-(auto a, auto b)
	requires (ScopedEnum<decltype(a)> || ScopedEnum<decltype(b)>)
	{ return enum_operator_impl(std::minus(), a, b); }
constexpr auto operator&(auto a, auto b)
	requires (ScopedEnum<decltype(a)> || ScopedEnum<decltype(b)>)
	{ return enum_operator_impl(std::bit_and(), a, b); }
constexpr auto operator|(auto a, auto b)
	requires (ScopedEnum<decltype(a)> || ScopedEnum<decltype(b)>)
	{ return enum_operator_impl(std::bit_or(), a, b); }
constexpr auto operator^(auto a, auto b)
	requires (ScopedEnum<decltype(a)> || ScopedEnum<decltype(b)>)
	{ return enum_operator_impl(std::bit_xor(), a, b); }

constexpr auto &operator+=(ScopedEnum auto &a, auto b) { return a = a + b; }
constexpr auto &operator-=(ScopedEnum auto &a, auto b) { return a = a - b; }
constexpr auto &operator&=(ScopedEnum auto &a, auto b) { return a = a & b; }
constexpr auto &operator|=(ScopedEnum auto &a, auto b) { return a = a | b; }
constexpr auto &operator^=(ScopedEnum auto &a, auto b) { return a = a ^ b; }

constexpr auto operator<=>(ScopedEnum auto a, auto b) { return std::to_underlying(a) <=> b; }
constexpr auto operator== (ScopedEnum auto a, auto b) { return a <=> b == 0; }

constexpr auto operator++(ScopedEnum auto &v)      { return v = v + decltype(auto(v)){1}; }
constexpr auto operator--(ScopedEnum auto &v)      { return v = v - decltype(auto(v)){1}; }
constexpr auto operator++(ScopedEnum auto &v, int) { const auto tmp = v; ++v; return tmp; }
constexpr auto operator--(ScopedEnum auto &v, int) { const auto tmp = v; --v; return tmp; }