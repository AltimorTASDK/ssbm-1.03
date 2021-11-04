#pragma once

#include <type_traits>

template<typename T, typename U = std::underlying_type_t<T>> requires std::is_scoped_enum_v<T>
constexpr T operator+(const T &a, const U &b)
{
	return static_cast<T>(static_cast<U>(a) + b);
}

template<typename T, typename U = std::underlying_type_t<T>> requires std::is_scoped_enum_v<T>
constexpr T operator+(const U &a, const T &b)
{
	return b + a;
}

template<typename T, typename U = std::underlying_type_t<T>> requires std::is_scoped_enum_v<T>
constexpr T operator+(const T &a, const T &b)
{
	return static_cast<T>(static_cast<U>(a) + static_cast<U>(a));
}

template<typename T, typename U = std::underlying_type_t<T>> requires std::is_scoped_enum_v<T>
constexpr T operator-(const T &a, const U &b)
{
	return static_cast<T>(static_cast<U>(a) - b);
}

template<typename T, typename U = std::underlying_type_t<T>> requires std::is_scoped_enum_v<T>
constexpr T operator-(const U &a, const T &b)
{
	return b - a;
}

template<typename T, typename U = std::underlying_type_t<T>> requires std::is_scoped_enum_v<T>
constexpr T operator-(const T &a, const T &b)
{
	return static_cast<T>(static_cast<U>(a) - static_cast<U>(a));
}

template<typename T, typename U = std::underlying_type_t<T>> requires std::is_scoped_enum_v<T>
constexpr T &operator++(T &value)
{
	return value = value + 1;
}

template<typename T, typename U = std::underlying_type_t<T>> requires std::is_scoped_enum_v<T>
constexpr T operator++(T &value, int)
{
	const auto result = value;
	++value;
	return result;
}

template<typename T, typename U = std::underlying_type_t<T>> requires std::is_scoped_enum_v<T>
constexpr T &operator--(T &value)
{
	return value = value - 1;
}

template<typename T, typename U = std::underlying_type_t<T>> requires std::is_scoped_enum_v<T>
constexpr T operator--(T &value, int)
{
	const auto result = value;
	--value;
	return result;
}