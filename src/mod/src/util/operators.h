#pragma once

#include <functional>

namespace operators {

// Aliases for the generic versions of the STL operator functors
constexpr auto add = std::plus();
constexpr auto sub = std::minus();
constexpr auto mul = std::multiplies();
constexpr auto div = std::divides();
constexpr auto mod = std::modulus();
constexpr auto neg = std::negate();

// Assignment operators
constexpr auto     eq = [](auto &a, auto &&b) -> decltype(auto) { return a  = b; };
constexpr auto add_eq = [](auto &a, auto &&b) -> decltype(auto) { return a += b; };
constexpr auto sub_eq = [](auto &a, auto &&b) -> decltype(auto) { return a -= b; };
constexpr auto mul_eq = [](auto &a, auto &&b) -> decltype(auto) { return a *= b; };
constexpr auto div_eq = [](auto &a, auto &&b) -> decltype(auto) { return a /= b; };
constexpr auto mod_eq = [](auto &a, auto &&b) -> decltype(auto) { return a %= b; };

// Allow min/max to be passed without overloading issues
constexpr auto min = [](auto &a, auto &&b) { return std::min(a, b); };
constexpr auto max = [](auto &a, auto &&b) { return std::max(a, b); };

} // namespace operators